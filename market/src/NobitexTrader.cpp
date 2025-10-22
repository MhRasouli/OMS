#include "NobitexTrader.h"
#include "UtilityTools.h"
#include "GlobalVariables.h"
#include "Database_manager.h"
#include <curl/curl.h>

NobitexTrader::NobitexTrader(Coin *_coin) : MarketTrader(_coin)
{
}

bool NobitexTrader::CheckTradeDone(int orderId, int tradeRequest_id, side_enum side, double quantity, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason)
{
    bool result = false;
    string srcCurrency = coin->name;
    std::transform(srcCurrency.begin(), srcCurrency.end(), srcCurrency.begin(), [](unsigned char c)
                   { return std::tolower(c); });

    cpr::Url url{UtilityTools::StringFormat(UtilityTools::string_attachEnd(GlobalVariables::markets[coin->market_id].host_trade,
                                                                           "market/trades/list?srcCurrency=%s&dstCurrency=%s"),
                                            srcCurrency == "irt" ? "usdt" : srcCurrency.c_str(),
                                            srcCurrency == "irt" ? "rls" : "usdt")
                     .c_str()};

    cpr::Session session_GetTrades;
    session_GetTrades.SetUrl(url);
    session_GetTrades.SetAcceptEncoding({{cpr::AcceptEncodingMethods::deflate, cpr::AcceptEncodingMethods::gzip, cpr::AcceptEncodingMethods::zlib}});
    session_GetTrades.SetHeader(cpr::Header{{"Authorization", UtilityTools::StringFormat("Token %s", GlobalVariables::markets[coin->market_id].password_securityKey.c_str()).c_str()}});

    //*******************************************************************
    session_GetTrades.PrepareGet();
    CURLcode curl_result = curl_easy_perform(session_GetTrades.GetCurlHolder()->handle);
    cpr::Response response_GetTrades = session_GetTrades.Complete(curl_result);

    // cpr::Response response_GetTrades = session_GetTrades.Get();
    // session_GetTrades.Delete();
    //*******************************************************************
    Json::Value root_GT;
    Json::Reader reader;
    reader.parse(response_GetTrades.text, root_GT);

    // cout << response_GetTrades.text << endl;

    if (root_GT["status"].asString() == "ok")
    {
        for (int i = 0; i < root_GT["trades"].size(); i++)
        {
            if (root_GT["trades"][i]["orderId"].asInt() == orderId)
            {
                marketResponse.symbol = root_GT["trades"][i]["market"].asString();
                marketResponse.orderId = root_GT["trades"][i]["orderId"].asString();
                marketResponse.clientOrderId = to_string(tradeRequest_id);
                marketResponse.transactTime = time(0); // UtilityTools::DatetimeString2Timestamp(root_GT["trades"][i]["timestamp"].asString());
                marketResponse.price = atof(root_GT["trades"][i]["price"].asString().c_str()) * GlobalVariables::coins[coin_id].market_price_coefficient;
                marketResponse.origQty = quantity;
                marketResponse.status = "FILLED";
                marketResponse.type = "MARKET";

                marketResponse.orderListId = 0;
                marketResponse.cummulativeQuoteQty = atof(root_GT["trades"][i]["total"].asString().c_str()) * GlobalVariables::coins[coin_id].market_price_coefficient;

                MarketResponseDetail marketResponseDetail;
                marketResponseDetail.price = atof(root_GT["trades"][i]["price"].asString().c_str()) * GlobalVariables::coins[coin_id].market_price_coefficient;
                marketResponseDetail.qty = atof(root_GT["trades"][i]["amount"].asString().c_str());
                marketResponseDetail.commission = atof(root_GT["trades"][i]["fee"].asString().c_str());
                marketResponseDetail.tradeId = root_GT["trades"][i]["id"].asInt();

                if (root_GT["trades"][i]["type"].asString() == "buy")
                {
                    marketResponse.executedQty = atof(root_GT["trades"][i]["amount"].asString().c_str()) - atof(root_GT["trades"][i]["fee"].asString().c_str());
                    marketResponseDetail.commissionAsset = srcCurrency == "irt" ? "USDT" : coin->name;
                    marketResponse.side = "BUY";

                    double qty_gottenUsdt = atof(root_GT["trades"][i]["amount"].asString().c_str()) - atof(root_GT["trades"][i]["fee"].asString().c_str());
                    double price_OneUsdt = atof(root_GT["trades"][i]["price"].asString().c_str()) * GlobalVariables::coins[coin_id].market_price_coefficient;

                    qty_res = UtilityTools::Get_quantity_DbRegion(qty_gottenUsdt, price_OneUsdt, coin_id);
                    price_res = UtilityTools::Get_price_DbRegion(price_OneUsdt, coin_id);
                }
                else
                {
                    marketResponse.executedQty = atof(root_GT["trades"][i]["total"].asString().c_str()) - atof(root_GT["trades"][i]["fee"].asString().c_str());
                    marketResponseDetail.commissionAsset = srcCurrency == "irt" ? "rls" : "USDT";
                    marketResponse.side = "SELL";

                    double qty_takenUsdt = (atof(root_GT["trades"][i]["total"].asString().c_str()) - atof(root_GT["trades"][i]["fee"].asString().c_str())) / atof(root_GT["trades"][i]["price"].asString().c_str());
                    double price_OneUsdt = atof(root_GT["trades"][i]["price"].asString().c_str()) * GlobalVariables::coins[coin_id].market_price_coefficient;

                    qty_res = UtilityTools::Get_quantity_DbRegion(qty_takenUsdt, price_OneUsdt, coin_id);
                    price_res = UtilityTools::Get_price_DbRegion(price_OneUsdt, coin_id);
                }
                marketResponse.lst_marketResponseDetail.push_back(marketResponseDetail);

                result = true;
                break;
            }
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
    }
    else
    {
        Logger::logger.log_special(LogLocation, true, "Nobitex order '%i' failed. errmsg='%s'", tradeRequest_id, root_GT["detail"].asString().c_str());
        rejectReason = root_GT["detail"].asString();
        result = false;
    }

    return result;
}

bool NobitexTrader::MarketOrder(int tradeRequest_id, side_enum side, double quantity, double commission_qty, double price, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason)
{
    bool result = false;
    side = UtilityTools::Get_side_DbRegion(side, coin_id);

    if (UtilityTools::Coin_is_UsdtBase(coin_id))
    {
        if (side == side_enum::Sell)
            quantity = UtilityTools::Get_quantity_DbRegion(quantity - commission_qty, price, coin_id);
        else
            quantity = UtilityTools::Get_quantity_DbRegion(quantity, price, coin_id);
    }
    else
    {
        if (side == side_enum::Buy)
            quantity = UtilityTools::Get_quantity_DbRegion(quantity - commission_qty, price, coin_id);
        else
            quantity = UtilityTools::Get_quantity_DbRegion(quantity, price, coin_id);
    }

    cpr::Url url{UtilityTools::string_attachEnd(GlobalVariables::markets[coin->market_id].host_trade, "market/orders/add").c_str()};
    cpr::Session session_PostNewOrder;
    session_PostNewOrder.SetUrl(url);
    session_PostNewOrder.SetAcceptEncoding({{cpr::AcceptEncodingMethods::deflate, cpr::AcceptEncodingMethods::gzip, cpr::AcceptEncodingMethods::zlib}});

    session_PostNewOrder.SetHeader(cpr::Header{
        {"Authorization", UtilityTools::StringFormat("Token %s", GlobalVariables::markets[coin->market_id].password_securityKey.c_str()).c_str()},
        {"content-type", "application/json"}});

    string srcCurrency = coin->name;
    std::transform(srcCurrency.begin(), srcCurrency.end(), srcCurrency.begin(), [](unsigned char c)
                   { return std::tolower(c); });

    string str_precision = "%.";
    str_precision += srcCurrency == "irt" ? "2" : to_string(coin->quantity_precision);
    str_precision += "f";

    string str_side = side == side_enum::Buy ? "buy" : "sell";

    string body = UtilityTools::StringFormat("{\"type\":\"%s\",\"srcCurrency\":\"%s\",\"dstCurrency\":\"%s\",\"amount\":%s,\"execution\": \"market\",\"clientOrderId\":\"%s\"}",
                                             str_side.c_str(),
                                             srcCurrency == "irt" ? "usdt" : srcCurrency.c_str(),
                                             srcCurrency == "irt" ? "rls" : "usdt",
                                             srcCurrency == "irt" ? UtilityTools::StringFormat(str_precision.c_str(), UtilityTools::Ceil(quantity, 2)).c_str() : UtilityTools::StringFormat(str_precision.c_str(), UtilityTools::Ceil(quantity, coin->quantity_precision)).c_str(),
                                             to_string(tradeRequest_id).c_str());

    session_PostNewOrder.SetBody(cpr::Body{body.c_str()});

    //*******************************************************************
    session_PostNewOrder.PreparePost();
    CURLcode curl_result = curl_easy_perform(session_PostNewOrder.GetCurlHolder()->handle);
    cpr::Response response = session_PostNewOrder.Complete(curl_result);

    // cpr::Response response = session_PostNewOrder.Post();
    // session_PostNewOrder.Delete();
    //*******************************************************************
    Json::Value root;
    Json::Reader reader;
    reader.parse(response.text, root);

    // cout<<response.text<<endl;

    if (root["status"].asString() == "ok")
    {
        int orderId = root["order"]["id"].asInt();
        for (int i = 0; i < 5; i++)
        {
            this_thread::sleep_for(chrono::milliseconds(20));
            if (CheckTradeDone(orderId, tradeRequest_id, side, quantity, coin_id, qty_res, price_res, marketResponse, rejectReason))
            {
                result = true;
                break;
            }
        }

        if (!result)
        {
            Logger::logger.log_special(LogLocation, true, "Nobitex order '%i' failed. could not load trade result from nobitex", tradeRequest_id);
            rejectReason = "could not load trade result from market";
        }
    }
    else
    {
        Logger::logger.log_special(LogLocation, true, "Nobitex order '%i' failed. errmsg='%s'", tradeRequest_id, root["detail"].asString().c_str());
        rejectReason = root["detail"].asString();
        result = false;
    }

    return result;
}