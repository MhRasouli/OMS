#include "KucoinWebSocketFeed.h"
#include <iostream>
#include "Logger.h"

KucoinWebSocketFeed::KucoinWebSocketFeed(const std::string &url, const std::vector<std::string> &symbols)
    : m_url(url), m_symbols(symbols) {}

KucoinWebSocketFeed::~KucoinWebSocketFeed()
{
    stop();
    wait();
}

void KucoinWebSocketFeed::set_price_callback(PriceUpdateCallback callback)
{
    m_price_callback = callback;
}

void KucoinWebSocketFeed::run()
{
    // std::cout << "Starting WebSocket connection to: " << m_url << std::endl;
    try
    {
        m_client.init_asio();

        m_client.clear_access_channels(websocketpp::log::alevel::all);
        m_client.clear_error_channels(websocketpp::log::elevel::all);

        m_client.set_tls_init_handler([](connection_hdl)
                                      {
            //std::cout << "TLS context initialized" << std::endl;
            return std::make_shared<websocketpp::lib::asio::ssl::context>(
                websocketpp::lib::asio::ssl::context::tlsv12_client
            ); });

        m_client.set_open_handler([this](connection_hdl hdl)
                                  {
            //std::cout << "Connected to Kucoin WebSocket successfully!" << std::endl;
            m_connected = true;
            m_current_connection = hdl;
            subscribe(hdl);
            start_ping_loop(hdl); });

        m_client.set_close_handler([this](connection_hdl hdl)
                                   {
            //std::cout << "Disconnected from Kucoin WebSocket." << std::endl;
            m_connected = false; });

        m_client.set_fail_handler([this](connection_hdl hdl)
                                  {
            //std::cout << "WebSocket connection failed." << std::endl;
            m_connected = false; });

        m_client.set_message_handler([this](connection_hdl hdl, ws_client::message_ptr msg)
                                     {
            //std::cout << "Received WebSocket message: " << msg->get_payload() << std::endl;
            handle_message(hdl, msg->get_payload()); });

        websocketpp::lib::error_code ec;
        ws_client::connection_ptr con = m_client.get_connection(m_url, ec);
        if (ec)
        {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return;
        }

        // std::cout << "Attempting to connect to WebSocket..." << std::endl;
        m_client.connect(con);
        // std::cout << "Starting WebSocket run loop..." << std::endl;
        m_client.run();
    }
    catch (const std::exception &e)
    {
        Logger::logger.log_special(LogLocation, true, "Exception in WebSocket run: %s",e.what());
        //std::cerr << "Exception in WebSocket run: " << e.what() << std::endl;
    }
}

// void KucoinWebSocketFeed::run_async()
// {
//     std::thread([this]()
//                 { this->run(); })
//         .detach();
// }

// void KucoinWebSocketFeed::stop()
// {
//     m_running = false;
//     m_connected = false;

//     if (m_ping_thread.joinable())
//     {
//         m_ping_thread.join();
//     }

//     try
//     {
//         if (m_connected)
//         {
//             m_client.close(m_current_connection, websocketpp::close::status::normal, "");
//         }
//     }
//     catch (...)
//     {
//         // Ignore errors during shutdown
//     }
// }

void KucoinWebSocketFeed::subscribe(connection_hdl hdl)
{
    // std::cout << "Preparing subscription for symbols: ";
    //  for (const auto& symbol : m_symbols) {
    //      std::cout << symbol << " ";
    //  }
    //  std::cout << std::endl;

    json sub = {
        {"id", "1"},
        {"type", "subscribe"},
        {"topic", "/market/ticker:"},
        {"privateChannel", false},
        {"response", true}};
    std::string topic = "/market/ticker:";
    for (size_t i = 0; i < m_symbols.size(); ++i)
    {
        topic += m_symbols[i];
        if (i != m_symbols.size() - 1)
            topic += ",";
    }
    sub["topic"] = topic;
    // std::cout << "Subscribing with: " << sub.dump() << std::endl;

    try
    {
        m_client.send(hdl, sub.dump(), websocketpp::frame::opcode::text);
        // std::cout << "Subscription message sent successfully" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error sending subscription: " << e.what() << std::endl;
    }
}

void KucoinWebSocketFeed::handle_message(connection_hdl hdl, const std::string &payload)
{
    try
    {
        auto j = json::parse(payload);

        if (j.contains("type") && j["type"] == "ping")
        {
            json pong = {{"id", j.value("id", "1")}, {"type", "pong"}};
            m_client.send(hdl, pong.dump(), websocketpp::frame::opcode::text);
            // std::cout << "Sent pong: " << pong.dump() << std::endl;
        }
        else if (j.contains("type") && j["type"] == "message")
        {
            if (j.contains("data"))
            {
                process_ticker_data(j["topic"].get<std::string>(), j["data"]);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error processing message: " << e.what() << std::endl;
    }
}

std::string KucoinWebSocketFeed::extractBaseSymbol(const std::string &input)
{
    // Find the colon and dash positions
    std::size_t colonPos = input.find(':');
    std::size_t dashPos = input.find('-');

    // Check if both positions are valid
    if (colonPos == std::string::npos || dashPos == std::string::npos || dashPos <= colonPos + 1)
    {
        return ""; // Invalid format
    }

    // Extract and return substring between ':' and '-'
    return input.substr(colonPos + 1, dashPos - colonPos - 1);
}

void KucoinWebSocketFeed::process_ticker_data(std::string symbol, const json &data)
{
    try
    {
        if (symbol != "" && data.contains("bestBid") && data.contains("bestAsk"))
        {
            symbol = extractBaseSymbol(symbol);
            double best_bid = std::stod(data["bestBid"].get<std::string>());
            double best_ask = std::stod(data["bestAsk"].get<std::string>());
            double best_bid_size = std::stod(data["bestBidSize"].get<std::string>());
            double best_ask_size = std::stod(data["bestAskSize"].get<std::string>());

            if (m_price_callback)
            {
                m_price_callback(symbol, best_bid, best_bid_size, best_ask, best_ask_size);
            }

            // std::cout << "Price update - Symbol: " << symbol
            //           << ", Best Bid: " << best_bid << " (Size: " << best_bid_size << ")"
            //           << ", Best Ask: " << best_ask << " (Size: " << best_ask_size << ")" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error processing ticker data: " << e.what() << std::endl;
    }
}

void KucoinWebSocketFeed::start_ping_loop(connection_hdl hdl)
{
    m_ping_thread = std::thread([this, hdl]()
                                {
        while (m_running && m_connected) {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            if (m_running && m_connected) {
                try {
                    json ping = {{"id", "1"}, {"type", "ping"}};
                    m_client.send(hdl, ping.dump(), websocketpp::frame::opcode::text);
                    //std::cout << "Sent ping: " << ping.dump() << std::endl;
                } catch (...) {
                    break;
                }
            }
        } });
}
//******************************************************************************************** */
void KucoinWebSocketFeed::stop()
{
    m_running = false;

    // Close ping thread if running
    if (m_ping_thread.joinable())
    {
        m_ping_thread.join();
    }

    try
    {
        if (m_connected)
        {
            websocketpp::lib::error_code ec;
            m_client.close(m_current_connection, websocketpp::close::status::normal, "", ec);
            if (ec) {
                std::cout << "Error closing connection: " << ec.message() << std::endl;
            }
        }

        // Stop the asio event loop
        m_client.stop();
    }
    catch (...)
    {
        // Ignore errors during shutdown
    }

    m_connected = false;
}
void KucoinWebSocketFeed::run_async()
{
    m_thread = std::thread([this]() { this->run(); });
}

void KucoinWebSocketFeed::wait()
{
    if (m_thread.joinable()) m_thread.join();
}