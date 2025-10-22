// BinanceTradingService.cpp : Defines the entry point for the application.
//

// mysql example
/*
#include "main.h"


int main()
{
    cout << endl;
    cout << "Running 'SELECT 'Hello World!' » AS _message ..." << endl;

        try {
        sql::Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::ResultSet* res;

        // Create a connection
        driver = get_driver_instance();
        con = driver->connect("tcp://192.168.56.101:3306", "root", "office3");
        // Connect to the MySQL test database
        con->setSchema("test");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
        while (res->next()) {
            cout << "\t... MySQL replies: ";
            // Access column data by alias or column name
            cout << res->getString("_message") << endl;
            cout << "\t... MySQL says it again: ";
            // Access column data by numeric offset, 1 is the first column
            cout << res->getString(1) << endl;
        }
        delete res;
        delete stmt;
        delete con;

    }
    catch (sql::SQLException& e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
            << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << endl;

    return EXIT_SUCCESS;
}
*/

// socket example (SERVER EXAMPLE)
/*
#include <iostream>
#include <csignal>

#include "socket/include/tcp_server.h"


// declare the server
TcpServer server;

// declare a server observer which will receive incomingPacketHandler messages.
// the server supports multiple observers
server_observer_t observer1, observer2;

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg1(const std::string& clientIP, const char* msg, size_t size) {
    std::string msgStr = msg;
    // print client message
    std::cout << "Observer1 got client msg: " << msgStr << "\n";
}

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg2(const std::string& clientIP, const char* msg, size_t size) {
    std::string msgStr = msg;
    // print client message
    std::cout << "Observer2 got client msg: " << msgStr << "\n";
}

// observer callback. will be called when client disconnects
void onClientDisconnected(const std::string& ip, const std::string& msg) {
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << "\n";
}

// accept a single client.
// if you wish to accept multiple clients, call this function in a loop
// (you might want to use a thread to accept clients without blocking)
void acceptClient() {
    try {
        std::cout << "waiting for incoming client...\n";
        std::string clientIP = server.acceptClient(0);
        std::cout << "accepted new client with IP: " << clientIP << "\n" <<
            "== updated list of accepted clients ==" << "\n";
        server.printClients();
    }
    catch (const std::runtime_error& error) {
        std::cout << "Accepting client failed: " << error.what() << "\n";
    }
}

void printMenu() {
    std::cout << "\n\nselect one of the following options: \n" <<
        "1. send all clients a message\n" <<
        "2. print list of accepted clients\n" <<
        "3. send message to a specific client\n" <<
        "4. close server and exit\n";
}

int getMenuSelection() {
    int selection = 0;
    std::cin >> selection;
    if (!std::cin) {
        throw std::runtime_error("invalid menu input. expected a number, but got something else");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return selection;
}


 // handle menu selection and return true in case program should terminate
 // after handling selection

bool handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 4;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
            ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
    }
    switch (selection) {
    case 1: { // send all clients a message
        std::string msg;
        std::cout << "type message to send to all connected clients:\n";
        getline(std::cin, msg);
        pipe_ret_t sendingResult = server.sendToAllClients(msg.c_str(), msg.size());
        if (sendingResult.isSuccessful()) {
            std::cout << "sent message to all clients successfully\n";
        }
        else {
            std::cout << "failed to sent message: " << sendingResult.message() << "\n";
        }
        break;
    }
    case 2: { // print list of accepted clients
        server.printClients();
        break;
    }
    case 3: { // send message to a specific client
        std::cout << "enter client IP:\n";
        std::string clientIP;
        std::cin >> clientIP;
        std::cout << "enter message to send:\n";
        std::string message;
        std::cin >> message;
        pipe_ret_t result = server.sendToClient(clientIP, message.c_str(), message.size());
        if (!result.isSuccessful()) {
            std::cout << "sending failed: " << result.message() << "\n";
        }
        else {
            std::cout << "sending succeeded\n";
        }
        break;
    };
    case 4: { // close server
        pipe_ret_t sendingResult = server.close();
        if (sendingResult.isSuccessful()) {
            std::cout << "closed server successfully\n";
        }
        else {
            std::cout << "failed to close server: " << sendingResult.message() << "\n";
        }
        return true;
    }
    default: {
        std::cout << "invalid selection: " << selection <<
            ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
    }
    }
    return false;
}

int main()
{
    // start server on port 65123
    pipe_ret_t startRet = server.start(65123);
    if (startRet.isSuccessful()) {
        std::cout << "Server setup succeeded\n";
    }
    else {
        std::cout << "Server setup failed: " << startRet.message() << "\n";
        return EXIT_FAILURE;
    }

    // configure and register observer1
    observer1.incomingPacketHandler = onIncomingMsg1;
    observer1.disconnectionHandler = onClientDisconnected;
    observer1.wantedIP = "127.0.0.1";
    server.subscribe(observer1);

    // configure and register observer2
    observer2.incomingPacketHandler = onIncomingMsg2;
    observer2.disconnectionHandler = nullptr; // nullptr or not setting this means we don't care about disconnection event
    observer2.wantedIP = "64.91.226.33"; // use empty string instead to receive messages from any IP address
    server.subscribe(observer2);

    acceptClient();

    bool shouldTerminate = false;
    while (!shouldTerminate) {
        printMenu();
        int selection = getMenuSelection();
        shouldTerminate = handleMenuSelection(selection);
    }

    return 0;
}
*/

// BINAPI example
/*
#include "include/websocket.hpp"
#include "include/api.hpp"
#include "include/pairslist.hpp"
#include "include/reports.hpp"
#include "include/flatjson.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <iostream>
#include <fstream>
#include "include/errors.hpp"

//###############################################################

std::string read_file(const char* fname) {
    std::ifstream file(fname);
    assert(file);

    std::string res{
         (std::istreambuf_iterator<char>(file))
        ,(std::istreambuf_iterator<char>())
    };

    return res;
}

//###############################################################

#define PRINT_IF_ERROR(res) \
    if ( !static_cast<bool>(res) ) { \
        std::cout << __FILE__ << "(" << __LINE__ << "): msg=" << res.errmsg << std::endl; \
    }

//###############################################################

#define BREAK_IF_ERROR(res) \
    PRINT_IF_ERROR(res); \
    if ( !res ) { \
        return EXIT_FAILURE; \
    }

//###############################################################

int main(int argc, char** argv) {
    //assert(argc == 3);
    const std::string pk = "x2FEe6rYflIBFzW7JfHoCgR7pp2coIhc426rVdikFJURxPXEzhSDvohM7BIwvFkY";// argv[1];
    const std::string sk = "KuEILG9Xp21MmQBgUnzYQqBagQGwVriRxkazO7PeEawlII7DV47OJ3Wvt5Db79Hk";// argv[2];

    std::cout.precision(8);

    boost::asio::io_context ioctx;
    binapi::ws::websockets wsp(ioctx, "stream.binance.com", "9443");

    binapi::rest::api api(
        ioctx
        , "api.binance.com"
        , "443"
        , pk
        , sk
        , 10000
    );

    static const char* testpair = "SHIBUSDT";


    if (false)
    {
        const std::string accinfo_str = read_file("accinfo.json");
        const std::string exinfo_str = read_file("exinfo.json");

        const flatjson::fjson exinfo_json{ exinfo_str.c_str(), exinfo_str.size() };
        binapi::rest::exchange_info_t exinfo = binapi::rest::exchange_info_t::construct(exinfo_json);

        const flatjson::fjson accinfo_json{ accinfo_str.c_str(), accinfo_str.size() };
        binapi::rest::account_info_t accinfo = binapi::rest::account_info_t::construct(accinfo_json);

        auto pairs0 = binapi::process_pairs("BTCUSDT", "", exinfo);
        assert(pairs0.size() == 1 && *(pairs0.begin()) == "BTCUSDT");

        auto pairs1 = binapi::process_pairs("BTCUSDT,BNBUSDT", "BTCUSDT", exinfo);
        assert(pairs1.size() == 1 && *(pairs1.begin()) == "BNBUSDT");

        auto pairs2 = binapi::process_pairs("*USDT", "BTCUSDT,BNBUSDT", exinfo);
        assert(pairs2.size() == 156 && pairs2.count("BTCUSDT") == 0 && pairs2.count("BNBUSDT") == 0);

        static const auto trades_report_cb = [](const binapi::rest::order_info_t& o) {
            std::cout << o.symbol << " - " << o.orderId << std::endl;
        };
        std::cout << "******************** BALANCE REPORT *********************************" << std::endl;
        binapi::make_balance_report(std::cout, api, accinfo, exinfo);
        std::cout << std::endl;
        std::cout << "********************* TRADES REPORT *********************************" << std::endl;
        //        binapi::make_trades_report(std::cout, api, accinfo, exinfo, {"ETHUSDT"}, trades_report_cb);
        //        binapi::make_trades_report(std::cout, api, accinfo, exinfo, {"*USDT"}, trades_report_cb, 0, "2018-11-12 17:38:29"); // "2018-11-12 17:38:29"
        binapi::make_trades_report_for_last_day(std::cout, api, accinfo, exinfo, { "ETHUSDT" }, trades_report_cb);

        std::cout << std::endl;
        std::cout << "******************* OPEN ORDERS REPORT ******************************" << std::endl;
        binapi::make_open_orders_report(std::cout, api, exinfo, {});

        std::cout << std::endl;
    }

    auto account = api.account_info();
    if (!binapi::rest::e_error_equal(account.ec, binapi::rest::e_error::OK)) {
        std::cout
            << "account_error: ec=" << account.ec
            << ", ename=" << binapi::rest::e_error_to_string(account.ec)
            << ", emsg=" << account.errmsg
            << std::endl << std::endl;

        return EXIT_FAILURE;
    }
    else {
        BREAK_IF_ERROR(account);
        std::cout << "account=" << account.v << std::endl << std::endl;
    }

    auto exinfo = api.exchange_info();
    BREAK_IF_ERROR(exinfo);
    std::cout << "exinfo=" << exinfo.v << std::endl << std::endl;

    auto ping = api.ping();
    BREAK_IF_ERROR(ping);
    std::cout << "ping=" << ping.v.ok << std::endl << std::endl;

    auto server_time = api.server_time();
    BREAK_IF_ERROR(server_time);
    std::cout << "server_time=" << server_time.v.serverTime << std::endl << std::endl;


    auto price = api.price(testpair);
    BREAK_IF_ERROR(price);
    std::cout << "price=" << price.v.price << std::endl << std::endl;

    auto prices = api.prices();
    BREAK_IF_ERROR(price);
    std::cout << "prices=" << prices.v << std::endl << std::endl;

    auto depths = api.depths(testpair, 5);
    BREAK_IF_ERROR(depths);
    std::cout << "depths=" << depths.v << std::endl << std::endl;

    auto trade = api.trade(testpair);
    BREAK_IF_ERROR(trade);
    std::cout << "trade=" << trade.v << std::endl << std::endl;

    auto trades = api.trades(testpair, 2);
    BREAK_IF_ERROR(trades);
    std::cout << "trades=" << trades.v << std::endl << std::endl;

    auto agg_trade = api.agg_trade(testpair);
    BREAK_IF_ERROR(agg_trade);
    std::cout << "agg_trade=" << agg_trade.v << std::endl << std::endl;

    auto agg_trades = api.agg_trades(testpair, 2);
    BREAK_IF_ERROR(agg_trades);
    std::cout << "agg_trades=" << agg_trades.v << std::endl << std::endl;

    auto klines = api.klines(testpair, "1m", 10);
    BREAK_IF_ERROR(klines);
    std::cout << "klines=" << klines.v << std::endl << std::endl;

    auto orderinfo = api.order_info("NULSUSDT", 3040219);
    BREAK_IF_ERROR(orderinfo);
    std::cout << "orderinfo=" << orderinfo.v << std::endl << std::endl;

    auto openorders = api.open_orders(testpair);
    BREAK_IF_ERROR(openorders);
    std::cout << "openorders=" << openorders.v << std::endl << std::endl;

    auto allorders = api.all_orders(testpair, 0, 0, 0, 5);
    BREAK_IF_ERROR(allorders);
    std::cout << "allorders=" << allorders.v << std::endl << std::endl;

    auto mytrades = api.my_trades(testpair, 0, 0, 0, 5);
    BREAK_IF_ERROR(mytrades);
    std::cout << "mytrades=" << mytrades.v << std::endl << std::endl;
    //******************************************************************************
        //auto neworder = api.new_order(
        //     "BNBUSDT"
        //    ,binapi::e_side::buy
        //    ,binapi::e_type::limit
        //    ,binapi::e_time::GTC
        //    ,binapi::e_trade_resp_type::FULL
        //    ,"0.1"
        //    ,"0"
        //    ,nullptr
        //    ,nullptr
        //    ,nullptr
        //);
        //BREAK_IF_ERROR(neworder);
        //std::cout << "neworder=" << neworder.v << std::endl << std::endl;

        //auto cancelorder = api.cancel_order("BNBUSDT", 1, nullptr, nullptr);
        //BREAK_IF_ERROR(cancelorder);
        //std::cout << "cancelorder=" << cancelorder.v << std::endl << std::endl;
        //******************************************************************************

    auto start_uds = api.start_user_data_stream();
    BREAK_IF_ERROR(start_uds);
    std::cout << "start_uds=" << start_uds.v << std::endl << std::endl;

    auto user_data_stream = wsp.userdata(start_uds.v.listenKey.c_str(),
        [](const char* fl, int ec, std::string errmsg, binapi::userdata::account_update_t msg) -> bool {
            if (ec) {
                std::cout << "account update: fl=" << fl << ", ec=" << ec << ", errmsg: " << errmsg << ", msg: " << msg << std::endl;
                return false;
            }

            std::cout << "account update:\n" << msg << std::endl;
            return true;
        }
        , [](const char* fl, int ec, std::string errmsg, binapi::userdata::balance_update_t msg) -> bool {
            if (ec) {
                std::cout << "balance update: fl=" << fl << ", ec=" << ec << ", errmsg: " << errmsg << ", msg: " << msg << std::endl;
                return false;
            }

            std::cout << "balance update:\n" << msg << std::endl;
            return true;
        }
            , [](const char* fl, int ec, std::string errmsg, binapi::userdata::order_update_t msg) -> bool {
            if (ec) {
                std::cout << "order update: fl=" << fl << ", ec=" << ec << ", errmsg: " << errmsg << ", msg: " << msg << std::endl;
                return false;
            }

            std::cout << "order update:\n" << msg << std::endl;
            return true;
        }
        );
    wsp.diff_depth(testpair, binapi::e_freq::_100ms,
        [](const char* fl, int ec, std::string errmsg, binapi::ws::diff_depths_t msg) -> bool {
            if (ec) {
                std::cout << "subscribe_depth(): fl=" << fl << ", ec=" << ec << ", errmsg: " << errmsg << ", msg: " << msg << std::endl;
                return false;
            }

            std::cout << "depth: " << msg << std::endl;
            return true;
        }
    );
    wsp.trade(testpair,
        [](const char* fl, int ec, std::string errmsg, binapi::ws::trade_t msg) -> bool {
            if (ec) {
                std::cout << "subscribe_trade(): fl=" << fl << ", ec=" << ec << ", errmsg: " << errmsg << ", msg: " << msg << std::endl;
                return false;
            }

            std::cout << "trade: " << msg << std::endl;
            return true;
        }
    );

    boost::asio::steady_timer unsubscribe_timer{ ioctx };
    unsubscribe_timer.expires_after(std::chrono::seconds{ 20 });
    unsubscribe_timer.async_wait(
        [user_data_stream, &wsp]
    (const boost::system::error_code&) {
            std::cout << "unsubscribe userdata stream" << std::endl;
            wsp.unsubscribe(user_data_stream);
        }
    );

    auto ping_uds = api.ping_user_data_stream(start_uds.v.listenKey);
    BREAK_IF_ERROR(ping_uds);
    std::cout << "ping_uds=" << ping_uds.v << std::endl << std::endl;

    ////    auto close_uds = api.close_user_data_stream(start_uds.v.listenKey);
    ////    BREAK_IF_ERROR(close_uds);
    ////    std::cout << "close_uds=" << close_uds.v << std::endl << std::endl;

    ioctx.run();

    return 0;
}

//###############################################################
*/

// https://github.com/rpclib/rpclib/
/*
#include "rpc/server.h"

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char *argv[]) {
// Creating a server that listens on port 8080
rpc::server srv(8080);

// Binding the name "foo" to free function foo.
// note: the signature is automatically captured
srv.bind("foo", &foo);

// Binding a lambda function to the name "add".
srv.bind("add", [](int a, int b) {
    return a + b;
});

// Run the server loop.
srv.run();

return 0;
}
//********************************************************************
#include "rpc/client.h"

int main() {
// Creating a client that connects to the localhost on port 8080
rpc::client client("127.0.0.1", 8080);

// Calling a function with paramters and converting the result to int
auto result = client.call("add", 2, 3).as<int>();
std::cout << "The result is: " << result << std::endl;
return 0;
}
*/

// https://github.com/libcpr/cpr
/*
#include <cpr/cpr.h>
#include "json/json.h"


int main(int argc, char **argv)
{
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                               cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
                               cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;            // 200
    r.header["content-type"]; // application/json; charset=utf-8
    r.text;                   // JSON text string
    return 0;
}
*/

// Main program

#include "stdafx.h"
#include "Configuration.h"
#include "Logger.h"
#include "GlobalVariables.h"
#include "Database_manager.h"
#include "ChangeProcessor.h"
#include <cstring>
#include "MemTracker.hpp"
#include "TradeRequestProcessor.h"
#include "MarketWalletsSupervisor.h"
#include "PriceFeederManager.h"
#include "DepositWithdrawalProcessor.h"
#include "ApiInterface.h"
#include "PythonInterface.h"

#include "KucoinTrader.h"
#include "Coin.h"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <Market.h>
#include "MarketObjects.h"

/**
 * @brief Checks if a key has been pressed on the keyboard without blocking
 * 
 * This function configures the terminal to non-canonical mode and checks if
 * a character is available for reading from stdin. It temporarily modifies
 * terminal settings and restores them after checking.
 * 
 * @return int Returns 1 if a key has been pressed, 0 otherwise
 * 
 * @note This function modifies terminal settings temporarily and restores them
 * @note Uses termios and fcntl for terminal control
 */
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

/**
 * @brief Main entry point for the Binance Trading Service OMS application
 * 
 * This function initializes and starts all components of the trading service including:
 * - Logger system setup and log directory creation
 * - Configuration loading
 * - Database initialization and data loading
 * - Price feeder manager startup
 * - Change processor startup
 * - Market-specific components (Kucoin, Binance, etc.)
 * - API interface startup
 * - Main application loop with keyboard input monitoring
 * - Graceful shutdown of all components
 * 
 * @return int Returns 0 on successful execution, 1 on initialization failure
 * 
 * @note Creates log directory with permissions: S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
 * @note Loads all database tables into global variables
 * @note Starts market-specific threads for enabled markets (market_id > 1)
 * @note Main loop runs until keyboard input is detected
 * @note Performs graceful shutdown of all started components
 */
int main()
{

    //************************************************************************************************** */
    std::cout.precision(8);

    char chr_path[256];
    getcwd(chr_path, 256);
    std::string programPath(chr_path);
    std::string programLogFolder = programPath + "/log/";
    int status = mkdir(programLogFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    Logger::logger.Initialize(programLogFolder);
    Logger::logger.log_special(LogLocation, false, "------------------ OMS TRADING SERVICE ------------------ version: %s", "1.6.31");

    Configuration::LoadConfig();

    if (!Database_manager::Initialize())
    {
        Logger::logger.log_special(LogLocation, false, "Please fix the issue and restart the application");
        return 1;
    }

    Database_manager::lastTradePriceGenerator_dbt.SelectAll(GlobalVariables::lastTradePriceGenerators);
    Database_manager::tradeRequestRobot_dbt.SelectAll(GlobalVariables::tradeRequestRobots);
    Database_manager::groupCoinMarketCommission_dbt.SelectAll(GlobalVariables::GroupCoinMarketCommissions);
    Database_manager::buyLimitation_dbt.SelectAll(GlobalVariables::buyLimitations);
    Database_manager::group_dbt.SelectAll(GlobalVariables::groups);
    Database_manager::stairCommission_dbt.SelectAll(GlobalVariables::stairCommissions);
    Database_manager::commissionRate_dbt.SelectAll(GlobalVariables::commissionRates);
    Database_manager::commissionRateRule_dbt.SelectAll(GlobalVariables::commissionRateRules);
    Database_manager::market_dbt.SelectAll(GlobalVariables::markets);
    Database_manager::account_dbt.SelectAll(GlobalVariables::accounts);
    Database_manager::tradeRequestGenerator_dbt.SelectAll(GlobalVariables::tradeRequestGenerators);
    Database_manager::coinMinBalance_alertMessageThreshold_dbt.SelectAll(GlobalVariables::coinMinBalanceThresholds);
    Database_manager::coin_dbt.SelectAll(GlobalVariables::coins);
    Database_manager::depositWithdrawal_dbt.SelectAll(GlobalVariables::depositWithdrawals);

    GlobalVariables::priceFeederManager.Start("WebSocketBoxManager");

    ChangeProcessor changeProcessor;
    changeProcessor.Start("ChangeProcessor");

    // run enable markets threads *****************************************************************
    for (const auto &market : GlobalVariables::markets)
    {
        if (market.second.market_id > 1 && market.second.enable)
        {
            MarketObjects marketObjects;
            if (market.second.name == "Kucoin")
            {
                marketObjects.pythonInterface=new PythonInterface(market.second);
                marketObjects.pythonInterface->Start("PythonInterface");
            }

            marketObjects.marketWalletsSupervisor=new MarketWalletsSupervisor(market.second);
            marketObjects.marketWalletsSupervisor->Start("MarketWalletsSupervisor");

            marketObjects.DW_Processor=new DepositWithdrawalProcessor(market.second);
            marketObjects.DW_Processor->Start("DepositWithdrawalProcessor");

            GlobalVariables::marketObjects[market.second.market_id]=marketObjects;
        }
    }

    //*****************************************************************

    ApiInterface apiInterface;
    apiInterface.Start("ApiInterface");

    while (!kbhit())
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    GlobalVariables::priceFeederManager.Stop();
    changeProcessor.Stop();
    apiInterface.Stop();

    for (auto &marketObjects : GlobalVariables::marketObjects)
    {
        if (marketObjects.second.pythonInterface)
            marketObjects.second.pythonInterface->Stop();

        if (marketObjects.second.marketWalletsSupervisor)
            marketObjects.second.marketWalletsSupervisor->Stop();

        if (marketObjects.second.DW_Processor)
            marketObjects.second.DW_Processor->Stop();

        marketObjects.second.FreeMemory();
    }

    return 0;
}
