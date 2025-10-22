#include "MarketWalletsSupervisor.h"
#include "Database_manager.h"
#include "Message.h"
#include "Logger.h"
#include "Message.h"

MarketWalletsSupervisor::MarketWalletsSupervisor(Market _market) : market(_market)
{

    if (market.name == "Binance")
    {
        api = new binapi::rest::api(ioctx, "api.binance.com", "443", market.username_privateKey, market.password_securityKey, 10000);
    }
    else if (market.name == "Nobitex")
    {
        cpr::Url url{UtilityTools::string_attachEnd(market.host_trade, "users/wallets/list").c_str()};

        session.SetUrl(url);
        session.SetAcceptEncoding({{cpr::AcceptEncodingMethods::deflate, cpr::AcceptEncodingMethods::gzip, cpr::AcceptEncodingMethods::zlib}});
        session.SetHeader(cpr::Header{{"Authorization", UtilityTools::StringFormat("Token %s", market.password_securityKey.c_str()).c_str()}});
    }
}

void MarketWalletsSupervisor::Loop_delay()
{
    static time_t time_lastGottentime = time(0);

    do
    {
        this_thread::sleep_for(chrono::seconds(1));
    } while (!isStopEventSignaled() && (time(0) - time_lastGottentime < market.checkMarketWalletBalanceEvery_sec));

    time_lastGottentime = time(0);
}

void MarketWalletsSupervisor::Get_MarketWalletBalance()
{
    std::lock_guard guard(mutex_);

    if (market.name == "Binance")
    {
        Get_MarketWalletBalance_Binance();
    }
    else if (market.name == "Nobitex")
    {
        Get_MarketWalletBalance_Nobitex();
    }
    else if (market.name == "Kucoin")
    {
        Get_MarketWalletBalance_kucoin();
    }
}

unsigned MarketWalletsSupervisor::ThreadFunction()
{
    while (isStopEventSignaled() == false)
    {
        Get_MarketWalletBalance();

        PublishMessages();

        Loop_delay();
    }
    return 0;
}

void MarketWalletsSupervisor::Get_MarketWalletBalance_Nobitex()
{
    try
    {
        session.PrepareGet();
        CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
        cpr::Response response = session.Complete(curl_result);
        // cpr::Response response = session.Get();

        Json::Value root;
        Json::Reader reader;

        reader.parse(response.text, root);

        if (root["status"].asString() == "ok")
        {
            for (int i = 0; i < root["wallets"].size(); i++)
            {
                if (root["wallets"][i]["currency"].asString() == "usdt")
                {
                    SetUsdtBalance(atof(root["wallets"][i]["balance"].asString().c_str()));
                }
                else
                {
                    for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
                    {
                        if (it_coin->second.market_id != market.market_id)
                            continue;

                        string coin_name = it_coin->second.name == "IRT" ? "rls" : UtilityTools::Get_LowerCharString(it_coin->second.name);
                        if (root["wallets"][i]["currency"].asString() == coin_name)
                        {
                            SetCoinBalance(it_coin->second, atof(root["wallets"][i]["balance"].asString().c_str()) * it_coin->second.market_price_coefficient);
                        }
                    }
                }
            }
        }
    }
    CATCH_STATEMENT
}

void MarketWalletsSupervisor::SetUsdtBalance(double Usdt_balance)
{
    WalletHistory walletHistory;

    walletHistory.account_id = market.accountID_MarketWalletBalance;
    walletHistory.trade_id = 0;
    walletHistory.balanceBefore_usd = GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt;
    walletHistory.balanceAfter_usd = Usdt_balance;
    walletHistory.coin_id = 0;
    walletHistory.coinBefore_qty = 0;
    walletHistory.coinAfter_qty = 0;
    walletHistory.time = time(0);

    if (GlobalVariables::accounts.find(market.accountID_MarketWalletBalance) != GlobalVariables::accounts.end())
    {
        if (GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt != walletHistory.balanceAfter_usd)
        {
            Database_manager::walletHistory_dbt.Insert(walletHistory, true);
            GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt = walletHistory.balanceAfter_usd;
        }
    }
    else
    {
        Database_manager::walletHistory_dbt.Insert(walletHistory, false);
    }
}

void MarketWalletsSupervisor::SetCoinBalance(Coin &coin, double coin_balance)
{
    WalletHistory walletHistory;
    walletHistory.account_id = market.accountID_MarketWalletBalance;
    walletHistory.trade_id = 0;
    walletHistory.balanceBefore_usd = GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt;
    walletHistory.balanceAfter_usd = GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt;
    walletHistory.coin_id = coin.coin_id;
    walletHistory.coinBefore_qty = coin.wallets[market.accountID_MarketWalletBalance].quantity;
    walletHistory.coinAfter_qty = coin_balance;
    walletHistory.time = time(0);
    if (coin.wallets.find(market.accountID_MarketWalletBalance) != coin.wallets.end())
    {
        if (coin.wallets[market.accountID_MarketWalletBalance].quantity != walletHistory.coinAfter_qty)
        {
            Database_manager::walletHistory_dbt.Insert(walletHistory, true);
            coin.wallets[market.accountID_MarketWalletBalance].quantity = walletHistory.coinAfter_qty;
        }
    }
    else
    {
        Database_manager::walletHistory_dbt.Insert(walletHistory, false);
    }
}

void MarketWalletsSupervisor::PublishMessages()
{
    if (GlobalVariables::accounts[market.accountID_TradeWith_TradeIsNotActive].balance_usdt < GlobalVariables::coinMinBalanceThresholds[1].tradeIsNotActive)
    {
        Message message;
        message.createTime = time(0);
        message.account_id = market.accountID_TradeWith_TradeIsNotActive;
        message.coin_id = 0;
        message.message = UtilityTools::StringFormat("The USDT balance of account %i is less than %.2f",
                                                     market.accountID_TradeWith_TradeIsNotActive,
                                                     GlobalVariables::coinMinBalanceThresholds[1].tradeIsNotActive);

        Database_manager::message_dbt.Insert(message);
    }
    if (GlobalVariables::accounts[market.accountID_TradeWith_LessThanMinTradePrice].balance_usdt < GlobalVariables::coinMinBalanceThresholds[1].lessThanMinTradePrice)
    {
        Message message;
        message.createTime = time(0);
        message.account_id = market.accountID_TradeWith_LessThanMinTradePrice;
        message.coin_id = 0;
        message.message = UtilityTools::StringFormat("The USDT balance of account %i is less than %.2f",
                                                     market.accountID_TradeWith_LessThanMinTradePrice,
                                                     GlobalVariables::coinMinBalanceThresholds[1].lessThanMinTradePrice);

        Database_manager::message_dbt.Insert(message);
    }
    if (GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt < GlobalVariables::coinMinBalanceThresholds[1].marketWalletBalance)
    {
        Message message;
        message.createTime = time(0);
        message.account_id = market.accountID_MarketWalletBalance;
        message.coin_id = 0;
        message.message = UtilityTools::StringFormat("The USDT balance of our account in %s market is %.2f and it is less than %.2f",
                                                     market.name.c_str(),
                                                     GlobalVariables::accounts[market.accountID_MarketWalletBalance].balance_usdt,
                                                     GlobalVariables::coinMinBalanceThresholds[1].marketWalletBalance);

        Database_manager::message_dbt.Insert(message);
    }

    for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin();
         it_coin != GlobalVariables::coins.end(); it_coin++)
    {
        if (it_coin->second.market_id != market.market_id)
            continue;

        if (GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].tradeIsNotActive > 0 &&
            (it_coin->second.wallets.find(market.accountID_TradeWith_TradeIsNotActive) == it_coin->second.wallets.end() ||
             it_coin->second.wallets[market.accountID_TradeWith_TradeIsNotActive].quantity <
                 GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].tradeIsNotActive))
        {
            Message message;
            message.createTime = time(0);
            message.account_id = market.accountID_TradeWith_TradeIsNotActive;
            message.coin_id = it_coin->second.coin_id;
            message.message = UtilityTools::StringFormat("The %s balance of account %i is less than %.2f",
                                                         it_coin->second.name.c_str(),
                                                         market.accountID_TradeWith_TradeIsNotActive,
                                                         GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].tradeIsNotActive);

            Database_manager::message_dbt.Insert(message);
        }

        if (GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].lessThanMinTradePrice > 0 &&
            (it_coin->second.wallets.find(market.accountID_TradeWith_LessThanMinTradePrice) == it_coin->second.wallets.end() ||
             it_coin->second.wallets[market.accountID_TradeWith_LessThanMinTradePrice].quantity <
                 GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].lessThanMinTradePrice))
        {
            Message message;
            message.createTime = time(0);
            message.account_id = market.accountID_TradeWith_LessThanMinTradePrice;
            message.coin_id = it_coin->second.coin_id;
            message.message = UtilityTools::StringFormat("The %s balance of account %i is less than %.2f",
                                                         it_coin->second.name.c_str(),
                                                         market.accountID_TradeWith_LessThanMinTradePrice,
                                                         GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].lessThanMinTradePrice);

            Database_manager::message_dbt.Insert(message);
        }

        if (GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].marketWalletBalance > 0 &&
            (it_coin->second.wallets.find(market.accountID_MarketWalletBalance) == it_coin->second.wallets.end() ||
             it_coin->second.wallets[market.accountID_MarketWalletBalance].quantity <
                 GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].marketWalletBalance))
        {
            Message message;
            message.createTime = time(0);
            message.account_id = market.accountID_MarketWalletBalance;
            message.coin_id = it_coin->second.coin_id;
            message.message = UtilityTools::StringFormat("The %s balance of our account in %s market is %.2f and it is less than %.2f",
                                                         it_coin->second.name.c_str(),
                                                         market.name.c_str(),
                                                         it_coin->second.wallets[market.accountID_MarketWalletBalance].quantity,
                                                         GlobalVariables::coinMinBalanceThresholds[it_coin->second.coinMinBalance_alertMessageThreshold_id].marketWalletBalance);

            Database_manager::message_dbt.Insert(message);
        }
    }
}

void MarketWalletsSupervisor::Get_MarketWalletBalance_Binance()
{
    try
    {
        auto walletsBalance = api->account_info();
        if (!binapi::rest::e_error_equal(walletsBalance.ec, binapi::rest::e_error::OK))
        {
            Logger::logger.log_special(LogLocation, true, "Reading coins balance from binance failed. ec=%i, ename=%s, emsg=%s", walletsBalance.ec, binapi::rest::e_error_to_string(walletsBalance.ec), walletsBalance.errmsg.c_str());
        }
        else
        {
            Logger::logger.log_special(LogLocation, false, "Reading coins balance from binance done successful");

            SetUsdtBalance((double)walletsBalance.v.balances["USDT"].free);

            double balance = 0;
            for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin();
                 it_coin != GlobalVariables::coins.end(); it_coin++)
            {
                if (it_coin->second.market_id != market.market_id)
                    continue;

                SetCoinBalance(it_coin->second, (double)walletsBalance.v.balances[it_coin->second.name].free);
            }
        }
    }
    CATCH_STATEMENT
}

void MarketWalletsSupervisor::Get_MarketWalletBalance_kucoin()
{
    std::packaged_task<std::map<std::string, std::string>()> task([]()
                                                                  {   std::map<std::string, std::string> cpp_map;
                                                                          try
                                                                          {
                                                                            const py::object py_obj =GlobalVariables::kucoinMarketInterface.attr("GetAccountList")(); 

                                                                              if (py::isinstance<py::list>(py_obj))
                                                                              {
                                                                                 py::list py_list = py_obj.cast<py::list>();
                                                                                  for (auto py_dict : py_list) 
                                                                                  {
                                                                                    std::string key="";
                                                                                    std::string value="";
                                                                                    std::string type="";
                                                                                    if (py::isinstance<py::dict>(py_dict))
                                                                                    {
                                                                                        for (auto item : py::cast<py::dict>(py_dict))
                                                                                        {
                                                                                                if((string)py::str(item.first)=="currency")
                                                                                                {
                                                                                                    key = py::str(item.second);
                                                                                                }
                                                                                                else  if((string)py::str(item.first)=="available")
                                                                                                {
                                                                                                    value = py::str(item.second);
                                                                                                }
                                                                                                else  if((string)py::str(item.first)=="type")
                                                                                                {
                                                                                                    type = py::str(item.second);
                                                                                                }
                                                                                        }
                                                                                    }
                                                                                  if(type=="trade" && key!="" && value!="")
                                                                                  {
                                                                                     cpp_map[key] = value;
                                                                                  }
                                                                                  }
                                                                              }
                                                                              else
                                                                              {
                                                                                Logger::logger.log_special(LogLocation, true, "The function did not return a list!");
                                                                                //std::cerr << "The function did not return a list!" << std::endl;
                                                                              }
                                                                          }
                                                                          catch (const py::error_already_set &e)
                                                                          {
                                                                            Logger::logger.log_special(LogLocation, true, "Python error: %s",e.what());
                                                                            //std::cerr << "Python error: " << e.what() << std::endl;
                                                                          }

                                                                          return cpp_map; });

    // Get future to retrieve the result
    std::future<std::map<std::string, std::string>> resultFuture = task.get_future();

    // Send the task to thread A via the task queue
    {
        std::lock_guard<std::mutex> lock(GlobalVariables::queueMutex);
        GlobalVariables::taskQueue.push(std::move(task));
    }

    // Notify thread A that a new task is available
    GlobalVariables::cv.notify_one();

    // Wait for the result from thread A
    std::map<std::string, std::string> response = resultFuture.get();

    //***************************************************************************************************** */
    for (const auto &pair : response)
    {
        if (pair.first == "USDT")
        {
            SetUsdtBalance(atof(pair.second.c_str()));
        }
        else
        {
            for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin();
                 it_coin != GlobalVariables::coins.end(); it_coin++)
            {
                if (it_coin->second.market_id != market.market_id)
                    continue;

                if (pair.first != it_coin->second.name)
                    continue;

                SetCoinBalance(it_coin->second, atof(pair.second.c_str()));
            }
        }
    }
}