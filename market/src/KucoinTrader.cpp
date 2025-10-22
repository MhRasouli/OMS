#include "KucoinTrader.h"
#include "UtilityTools.h"
#include "GlobalVariables.h"
#include "Database_manager.h"
#include "Logger.h"

KucoinTrader::KucoinTrader(Coin *_coin) : MarketTrader(_coin)
{
}

bool KucoinTrader::MarketOrder(int tradeRequest_id, side_enum side, double quantity, double commission_qty, double price, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason)
{
    bool result = false;

    string str_orderID = Send_market_order(tradeRequest_id, side, quantity);

    if (str_orderID != "" && Get_order_detail(str_orderID, quantity, qty_res, price_res, marketResponse))
    {
        result = true;
    }
    else
    {
        Logger::logger.log_special(LogLocation, true, "Kucoin order '%i' failed.", tradeRequest_id);
        result = false;
    }
    if (result)
    {
        if (Database_manager::marketResponse_dbt.Insert(marketResponse))
        {
            for (vector<MarketResponseDetail>::iterator it = marketResponse.lst_marketResponseDetail.begin();
                 it != marketResponse.lst_marketResponseDetail.end(); it++)
            {
                it->marketResponse_id = marketResponse.marketResponse_id;
                if (!Database_manager::marketResponseDetail_dbt.Insert(*it))
                    return false;
            }
        }
    }
    //***************************************************************************************************
    return result;
}

bool KucoinTrader::Get_order_detail(string orderId, double quantity, double &qty_res, double &price_res, MarketResponse &marketResponse)
{
    bool res = false;

    // Package the task for methodB with input
    std::packaged_task<std::map<std::string, std::string>()> task([orderId]()
                                                                  {
                                                                          std::map<std::string, std::string> cpp_map;
                                                                          try
                                                                          {
                                                                              const py::object py_obj = GlobalVariables::kucoinMarketInterface.attr("OrderDetail")(orderId);

                                                                              if (py::isinstance<py::dict>(py_obj))
                                                                              {
                                                                                  py::dict py_dict = py_obj.cast<py::dict>();

                                                                                  for (auto item : py_dict)
                                                                                  {
                                                                                      std::string key = py::str(item.first);    // Convert Python key to std::string
                                                                                      std::string value = py::str(item.second); // Convert Python value to std::string
                                                                                      cpp_map[key] = value;                     // Insert key-value pair into std::map
                                                                                  }
                                                                              }
                                                                              else
                                                                              {
                                                                                 Logger::logger.log_special(LogLocation, true, "The function did not return a dictionary!");
                                                                                  //std::cerr << "The function did not return a dictionary!" << std::endl;
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
    marketResponse.symbol = response["symbol"];
    marketResponse.orderId = response["id"];
    marketResponse.clientOrderId = response["clientOid"];
    marketResponse.transactTime = time(0); // response["createdAt"].cast<std::time_t>();
    marketResponse.price = (atof(response["dealFunds"].c_str()) / atof(response["dealSize"].c_str())) * coin->market_price_coefficient;
    marketResponse.origQty = quantity;
    marketResponse.status = "FILLED";
    marketResponse.type = "MARKET";

    marketResponse.orderListId = 0;
    marketResponse.cummulativeQuoteQty = atof(response["dealFunds"].c_str()) * coin->market_price_coefficient;

    MarketResponseDetail marketResponseDetail;
    marketResponseDetail.price = (atof(response["dealFunds"].c_str()) / atof(response["dealSize"].c_str())) * coin->market_price_coefficient;
    marketResponseDetail.qty = atof(response["dealSize"].c_str());
    marketResponseDetail.commission = atof(response["fee"].c_str());
    marketResponseDetail.tradeId = 0;

    response["side"] == "buy" ? marketResponse.side = "BUY" : marketResponse.side = "SELL";
    marketResponse.executedQty = atof(response["dealFunds"].c_str()) - atof(response["fee"].c_str());
    marketResponseDetail.commissionAsset = response["feeCurrency"];

    qty_res = atof(response["dealSize"].c_str());
    price_res = (atof(response["dealFunds"].c_str()) / atof(response["dealSize"].c_str())) * coin->market_price_coefficient;

    marketResponse.lst_marketResponseDetail.push_back(marketResponseDetail);
    res = true;

    return res;
}

string KucoinTrader::Send_market_order(int tradeRequest_id, side_enum side, double quantity)
{
    string res = "";

    string coin_name = coin->name;
    std::packaged_task<std::map<std::string, std::string>()> task([coin_name, tradeRequest_id, side, quantity]()
                                                                  {   std::map<std::string, std::string> cpp_map;
                                                                          try
                                                                          {
                                                                            const py::object py_obj =GlobalVariables::kucoinMarketInterface.attr("MarketOrder")(
                                                                            UtilityTools::string_attachEnd(coin_name, "-USDT").c_str(),
                                                                            to_string(tradeRequest_id).c_str(),
                                                                            side == side_enum::Buy ? "buy" : "sell",
                                                                            to_string(quantity).c_str()); 

                                                                              if (py::isinstance<py::dict>(py_obj))
                                                                              {
                                                                                  py::dict py_dict = py_obj.cast<py::dict>();

                                                                                  for (auto item : py_dict)
                                                                                  {
                                                                                      std::string key = py::str(item.first);    // Convert Python key to std::string
                                                                                      std::string value = py::str(item.second); // Convert Python value to std::string
                                                                                      cpp_map[key] = value;                     // Insert key-value pair into std::map
                                                                                  }
                                                                              }
                                                                              else
                                                                              {
                                                                                 Logger::logger.log_special(LogLocation, true, "The function did not return a dictionary!");
                                                                                  //std::cerr << "The function did not return a dictionary!" << std::endl;
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
    std::map<std::string, std::string> result = resultFuture.get();

    //***************************************************************************************************** */
    res = result["orderId"];

    return res;
}
