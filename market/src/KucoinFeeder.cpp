#include "KucoinFeeder.h"
#include "UtilityTools.h"
#include "Enums.h"
#include <iostream>
#include <thread>

KucoinFeeder::~KucoinFeeder()
{
    cleanup_websocket();
    fedchkThread.Stop();
    StopGetData();
}

KucoinFeeder::KucoinFeeder(Coin *Coin_ptr) : coin_ptr(Coin_ptr), fedchkThread(&dataIsNew, Coin_ptr, &mustDelete)
{
    // Initialize WebSocket components
    initialize_websocket();
}

void KucoinFeeder::RenewToken()
{
    StopGetData();
    initialize_websocket();

}

void KucoinFeeder::initialize_websocket()
{
    try
    {
        // std::cout << "Initializing WebSocket for coin: " << coin_ptr->name << std::endl;

        // Initialize token manager
        std::string api_url = "https://api.kucoin.com/api/v1/bullet-public";
        m_token_manager = std::make_unique<KucoinTokenManager>(api_url);

        // Start auto-renewal
        m_token_manager->start_auto_renew();

        // Wait a bit for initial token
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Get WebSocket URL
        std::string ws_url = m_token_manager->get_ws_url();
        // std::cout << "WebSocket URL: " << ws_url << std::endl;
        Logger::logger.log_special_if_verb(LogLocation, false, "Kucoin WebSocket URL obtained for %s", coin_ptr->name.c_str());

        // Prepare symbols for subscription
        std::vector<std::string> symbols = {UtilityTools::string_attachEnd(coin_ptr->name, "-USDT")};

        // Initialize WebSocket feed
        m_websocket_feed = std::make_unique<KucoinWebSocketFeed>(ws_url, symbols);

        // Set price callback
        m_websocket_feed->set_price_callback(
            [this](const std::string &symbol, double bid_price, double bid_quantity,
                   double ask_price, double ask_quantity)
            {
                this->on_price_update(symbol, bid_price, bid_quantity, ask_price, ask_quantity);
            });

        // Start the WebSocket feed in a separate thread
        m_websocket_feed->run_async();

        m_websocket_initialized = true;
        // std::cout << "WebSocket initialization completed for " << coin_ptr->name << std::endl;
        Logger::logger.log_special_if_verb(LogLocation, false, "Kucoin WebSocket initialized and started for %s", coin_ptr->name.c_str());
    }
    catch (const std::exception &e)
    {
        // std::cout << "Failed to initialize WebSocket for " << coin_ptr->name << ": " << e.what() << std::endl;
        Logger::logger.log_special(LogLocation, true, "Failed to initialize Kucoin WebSocket for %s: %s",
                                   coin_ptr->name.c_str(), e.what());
    }
}

void KucoinFeeder::cleanup_websocket()
{
    if (m_websocket_feed)
    {
        m_websocket_feed->stop();
        m_websocket_feed->wait();
        m_websocket_feed.reset();
    }

    if (m_token_manager)
    {
        m_token_manager->stop_auto_renew();
        m_token_manager.reset();
    }

    m_websocket_initialized = false;
}

void KucoinFeeder::on_price_update(const std::string &symbol, double bid_price, double bid_quantity,
                                   double ask_price, double ask_quantity)
{
    try
    {
        // Check if this update is for our coin
        if (symbol == coin_ptr->name)
        {
            // Update coin prices
            coin_ptr->market_buy_price = bid_price;
            coin_ptr->market_buy_quantity = bid_quantity;
            coin_ptr->market_sell_price = ask_price;
            coin_ptr->market_sell_quantity = ask_quantity;
            coin_ptr->time_price = time(0);

            dataIsNew = true;

            Logger::logger.log_special_if_verb(LogLocation, false, "Price update for %s: BidPrice=%.8f, BidQuantity=%.8f, AskPrice=%.8f, AskQuantity=%.8f",
                                               coin_ptr->name.c_str(), bid_price, bid_quantity, ask_price, ask_quantity);
        }
        else
        {
            Logger::logger.log_special(LogLocation, true, "Symbol mismatch %s != %s", symbol.c_str(), coin_ptr->name.c_str());
        }
    }
    catch (const std::exception &e)
    {
        Logger::logger.log_special(LogLocation, true, "Error processing price update for %s: %s", coin_ptr->name.c_str(), e.what());
    }
}

unsigned KucoinFeeder::ThreadFunction()
{
    fedchkThread.Start("Feed checker symbol: " + coin_ptr->name);

    while (isStopEventSignaled() == false)
    {
        try
        {
            // Check if WebSocket is initialized
            if (!m_websocket_initialized)
            {
                Logger::logger.log_special(LogLocation, false, "Reinitializing WebSocket for %s", coin_ptr->name.c_str());
                cleanup_websocket();
                initialize_websocket();
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            // Start WebSocket feed if not already running
            if (m_websocket_feed)
            {
                // The WebSocket feed runs in its own thread, we just need to keep this thread alive
                // and monitor the connection status
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            // The actual price updates come through the callback, so we don't need to poll here
            // Just keep the thread alive and monitor for any issues
        }
        catch (const std::runtime_error &re)
        {
            Logger::logger.log_special(LogLocation, true, "Runtime error in KucoinFeeder for %s: %s",
                                       coin_ptr->name.c_str(), re.what());
        }
        catch (const std::exception &ex)
        {
            Logger::logger.log_special(LogLocation, true, "Error in KucoinFeeder for %s: %s",
                                       coin_ptr->name.c_str(), ex.what());
        }
        catch (...)
        {
            Logger::logger.log_special(LogLocation, true, "Unknown failure in KucoinFeeder for %s. Possible memory corruption",
                                       coin_ptr->name.c_str());
        }

        Loop_delay();
    }

    return 0;
}

void KucoinFeeder::StopGetData()
{
    Stop();
    cleanup_websocket();
    //Stop();
}

void KucoinFeeder::Loop_delay()
{
    static time_t time_lastGottentime = time(0);
    do
    {
        this_thread::sleep_for(chrono::milliseconds(50));
    } while (time(0) - time_lastGottentime < 1);

    time_lastGottentime = time(0);
}
