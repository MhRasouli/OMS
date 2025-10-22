#include "ApiInterface.h"
#include "json/json.h"
#include "Coin.h"
#include "TradeRequestProcessor.h"
#include "DepositWithdrawal.h"

ApiServer::ApiServer(HttpServer &server) : AbstractServer<ApiServer>(server)
{
  this->bindAndAddMethod(Procedure("Select_coin_price", PARAMS_BY_NAME, JSON_STRING,
                                   "coin_name", JSON_STRING,
                                   NULL),
                         &ApiServer::Select_coin_price);

  this->bindAndAddMethod(Procedure("Select_coin", PARAMS_BY_NAME, JSON_STRING,
                                   "coin_name", JSON_STRING,
                                   NULL),
                         &ApiServer::Select_coin);

  this->bindAndAddMethod(Procedure("Select_coin_all", PARAMS_BY_NAME, JSON_STRING,
                                   NULL),
                         &ApiServer::Select_coin_all);

  this->bindAndAddMethod(Procedure("Select_wallet_balance", PARAMS_BY_NAME, JSON_STRING,
                                   "coin_name", JSON_STRING,
                                   "account_id", JSON_INTEGER,
                                   NULL),
                         &ApiServer::Select_wallet_balance);

  this->bindAndAddMethod(Procedure("Select_wallet_balance_all_coins", PARAMS_BY_NAME, JSON_STRING,
                                   "account_id", JSON_INTEGER,
                                   NULL),
                         &ApiServer::Select_wallet_balance_all_coins);

  this->bindAndAddMethod(Procedure("Select_tradeRequests_account", PARAMS_BY_NAME, JSON_STRING,
                                   "account_id", JSON_INTEGER,
                                   NULL),
                         &ApiServer::Select_tradeRequests_account);

  this->bindAndAddMethod(Procedure("Select_tradeRequest", PARAMS_BY_NAME, JSON_STRING,
                                   "tradeRequest_id", JSON_INTEGER,
                                   NULL),
                         &ApiServer::Select_tradeRequest);

  this->bindAndAddMethod(Procedure("Insert_tradeRequest", PARAMS_BY_NAME, JSON_STRING,
                                   "tradeRequest_obj", JSON_OBJECT,
                                   NULL),
                         &ApiServer::Insert_tradeRequest);

  this->bindAndAddMethod(Procedure("Update_tradeRequest", PARAMS_BY_NAME, JSON_STRING,
                                   "tradeRequest_obj", JSON_OBJECT,
                                   NULL),
                         &ApiServer::Update_tradeRequest);

  this->bindAndAddMethod(Procedure("Insert_depositWithdrawal", PARAMS_BY_NAME, JSON_STRING,
                                   "depositWithdrawal_obj", JSON_OBJECT,
                                   NULL),
                         &ApiServer::Insert_depositWithdrawal);

  this->bindAndAddMethod(Procedure("Update_coin_price", PARAMS_BY_NAME, JSON_STRING,
                                   "coin_name", JSON_STRING,
                                   "bid", JSON_REAL,
                                   "ask", JSON_REAL,
                                   NULL),
                         &ApiServer::Update_coin_price);
}

int ApiServer::CoinName_to_coinId(string coinName)
{
  int res = -1;
  if (coinName == "USDT")
  {
    res = 0;
  }
  else
  {
    for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
    {
      if (it_coin->second.name == coinName)
      {
        res = it_coin->second.coin_id;
        break;
      }
    }
  }
  if (res == -1)
  {
    string errorMessage = "coin '";
    errorMessage += coinName;
    errorMessage += "' is not exist/active";

    throw std::runtime_error(errorMessage);
  }

  return res;
}

string ApiServer::CoinId_to_coinName(int coinId)
{
  string res = "";
  if (GlobalVariables::coins.find(coinId) != GlobalVariables::coins.end())
  {
    res = GlobalVariables::coins[coinId].name;
  }

  return res;
}
//***************************************************************************************************

Json::Value ApiServer::TradeRequest_to_jsonValue(TradeRequest tradeRequest)
{
  Json::Value res;

  res["tradeRequest_id"] = tradeRequest.tradeRequest_id;
  res["account_id"] = tradeRequest.account_id;
  res["coin_name"] = CoinId_to_coinName(tradeRequest.coin_id);
  res["quantity"] = tradeRequest.get_quantity_DbRegion();
  res["side"] = tradeRequest.get_side_DbRegion() == side_enum::Buy ? "B" : "S";
  res["price"] = tradeRequest.get_price_DbRegion();
  res["isLock"] = tradeRequest.isLock;
  res["status"] = (int)tradeRequest.status;
  res["time"] = UtilityTools::Timestamp2DatetimeString(tradeRequest.time);
  res["coinPair"] = tradeRequest.coinPair;
  res["type"] = (int)tradeRequest.type;
  res["visible"] = tradeRequest.visible;

  return res;
}

TradeRequest ApiServer::JsonValue_to_tradeRequest(const Json::Value &JsonValue)
{
  TradeRequest tradeRequest;
  try
  {
    tradeRequest.tradeRequest_id = JsonValue.isMember("tradeRequest_id") ? JsonValue["tradeRequest_id"].asInt() : throw std::runtime_error("field 'tradeRequest_id' is not exist in json request");
    tradeRequest.account_id = JsonValue.isMember("account_id") ? JsonValue["account_id"].asInt() : throw std::runtime_error("field 'account_id' is not exist in json request");
    tradeRequest.coin_id = JsonValue.isMember("coin_name") ? CoinName_to_coinId(JsonValue["coin_name"].asString()) : throw std::runtime_error("field 'coin_name' is not exist in json request");
    tradeRequest.set_quantity_DbRegion(JsonValue.isMember("quantity") ? JsonValue["quantity"].asDouble() : throw std::runtime_error("field 'quantity' is not exist in json request"));
    tradeRequest.set_side_DbRegion(JsonValue.isMember("side") ? (JsonValue["side"].asString() == "B" ? side_enum::Buy : side_enum::Sell) : throw std::runtime_error("field 'side' is not exist in json request"));
    tradeRequest.set_price_DbRegion(JsonValue.isMember("price") ? JsonValue["price"].asDouble() : throw std::runtime_error("field 'price' is not exist in json request"));
    tradeRequest.isLock = JsonValue.isMember("isLock") ? JsonValue["isLock"].asBool() : throw std::runtime_error("field 'isLock' is not exist in json request");
    tradeRequest.status = JsonValue.isMember("status") ? (status_enum)JsonValue["status"].asInt() : throw std::runtime_error("field 'status' is not exist in json request");
    tradeRequest.time = JsonValue.isMember("time") ? UtilityTools::DatetimeString2Timestamp(JsonValue["time"].asString()) : throw std::runtime_error("field 'time' is not exist in json request");
    tradeRequest.coinPair = JsonValue.isMember("coin_name") ? JsonValue["coin_name"].asString() : throw std::runtime_error("field 'coin_name' is not exist in json request");
    tradeRequest.type = JsonValue.isMember("type") ? (type_enum)JsonValue["type"].asInt() : throw std::runtime_error("field 'type' is not exist in json request");
    tradeRequest.visible = JsonValue.isMember("visible") ? JsonValue["visible"].asBool() : throw std::runtime_error("field 'visible' is not exist in json request");
  }
  catch (const std::runtime_error &e)
  {
    throw std::runtime_error(e.what());
  }
  return tradeRequest;
}

void ApiServer::Select_tradeRequest(const Json::Value &request, Json::Value &response)
{
  // curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_tradeRequest\",\"id\":1,\"params\":{\"tradeRequest_id\":10549469}}" 144.217.82.52:8383

  int tradeRequest_id = request["tradeRequest_id"].asInt();
  bool tr_found = false;
  for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin();
       it_coin != GlobalVariables::coins.end(); it_coin++)
  {
#define coin_tradeRequests it_coin->second.tradeRequestProcessor->tradeRequests

    if (coin_tradeRequests.find(tradeRequest_id) != coin_tradeRequests.end())
    {
      response["tradeRequest_obj"] = TradeRequest_to_jsonValue(coin_tradeRequests[tradeRequest_id]);
      tr_found = true;
      break;
    }
  }

  if (!tr_found)
    response["Error"] = "tradeRequest is not exist/active";
}

void ApiServer::Select_tradeRequests_account(const Json::Value &request, Json::Value &response)
{
  // curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_tradeRequests_account\",\"id\":1,\"params\":{\"account_id\":46}}" 144.217.82.52:8383

  int account_id = request["account_id"].asInt();
  bool tr_found = false;
  for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
  {
#define coin_tradeRequests it_coin->second.tradeRequestProcessor->tradeRequests

    for (map<int, TradeRequest>::iterator it_tr = coin_tradeRequests.begin(); it_tr != coin_tradeRequests.end(); it_tr++)
    {
      if (it_tr->second.account_id == account_id)
      {
        response[to_string(it_tr->second.tradeRequest_id)] = TradeRequest_to_jsonValue(it_tr->second);
        tr_found = true;
      }
    }
  }

  if (!tr_found)
    response["Error"] = "there is no active tradeRequest";
}

void ApiServer::Update_tradeRequest(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Update_tradeRequest\",\"id\":1,\"params\":{\"tradeRequest_obj\":{\"tradeRequest_id\":10549711,\"account_id\":46,\"coin_name\":\"BTC\",\"quantity\":5.5,\"side\":\"B\",\"price\":7.8,\"isLock\":false,\"status\":0,\"time\":\"2023-04-06 07:16:13\",\"coinPair\":\"BNB\",\"type\":1,\"visible\":true}}}" 144.217.82.52:8383

  try
  {
    Json::Value tradeRequest_obj = request["tradeRequest_obj"];
    TradeRequest tradeRequest = JsonValue_to_tradeRequest(tradeRequest_obj);

    if (GlobalVariables::coins.find(tradeRequest.coin_id) != GlobalVariables::coins.end())
    {
      if (!Database_manager::tradeRequest_dbt.Update(tradeRequest, GlobalVariables::coins[tradeRequest.coin_id].tradeRequestProcessor->tradeRequests))
      {
        response["Error"] = "Insert into tradeRequest faild";
      }
      else
      {
        response["tradeRequest_id"] = to_string(tradeRequest.tradeRequest_id);
      }
    }
    else
    {
      response["Error"] = "coin is not exist";
    }
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

void ApiServer::Insert_tradeRequest(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Insert_tradeRequest\",\"id\":1,\"params\":{\"tradeRequest_obj\":{\"tradeRequest_id\":0,\"account_id\":46,\"coin_name\":\"BTC\",\"quantity\":5.5,\"side\":\"B\",\"price\":7.8,\"isLock\":false,\"status\":0,\"time\":\"2023-04-06 07:16:13\",\"coinPair\":\"BNB\",\"type\":1,\"visible\":true}}}" 144.217.82.52:8383

  // curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Insert_tradeRequest\",\"id\":1,\"params\":{\"tradeRequest_obj\":{\"tradeRequest_id\":0,\"account_id\":46,\"coin_name\":\"SHIB\",\"quantity\":1077375,\"side\":\"B\",\"price\":0,\"isLock\":false,\"status\":0,\"time\":\"2023-04-06 07:16:13\",\"type\":0,\"visible\":true}}}" 144.217.82.52:8383
  try
  {
    Json::Value tradeRequest_obj = request["tradeRequest_obj"];
    TradeRequest tradeRequest = JsonValue_to_tradeRequest(tradeRequest_obj);

    if (GlobalVariables::coins.find(tradeRequest.coin_id) != GlobalVariables::coins.end())
    {
      if (!Database_manager::tradeRequest_dbt.Insert(tradeRequest, GlobalVariables::coins[tradeRequest.coin_id].tradeRequestProcessor->enteredTradeRequests))
      {
        response["Error"] = "Insert into tradeRequest faild";
      }
      else
      {
        response["tradeRequest_id"] = to_string(tradeRequest.tradeRequest_id);
      }
    }
    else
    {
      response["Error"] = "coin is not exist";
    }
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

DepositWithdrawal ApiServer::JsonValue_to_depositWithdrawal(const Json::Value &JsonValue)
{
  DepositWithdrawal depositWithdrawal;
  try
  {
    depositWithdrawal.account_id = JsonValue.isMember("account_id") ? JsonValue["account_id"].asInt() : throw std::runtime_error("field 'account_id' is not exist in json request");
    depositWithdrawal.coin_id = JsonValue.isMember("coin_name") ? CoinName_to_coinId(JsonValue["coin_name"].asString()) : throw std::runtime_error("field 'coin_name' is not exist in json request");
    depositWithdrawal.quantity_request = JsonValue.isMember("quantity_request") ? JsonValue["quantity_request"].asDouble() : throw std::runtime_error("field 'quantity_request' is not exist in json request");
    depositWithdrawal.commission_quantity = JsonValue.isMember("commission_quantity") ? JsonValue["commission_quantity"].asDouble() : throw std::runtime_error("field 'commission_quantity' is not exist in json request");
    depositWithdrawal.type = JsonValue.isMember("type") ? (JsonValue["type"].asString() == "D" ? type_DW_enum::Deposit : type_DW_enum::Withdrawal) : throw std::runtime_error("field 'type' is not exist in json request");
    depositWithdrawal.time_request = JsonValue.isMember("time_request") ? UtilityTools::DatetimeString2Timestamp(JsonValue["time_request"].asString()) : throw std::runtime_error("field 'time_request' is not exist in json request");
    depositWithdrawal.network_fee_qty = JsonValue.isMember("network_fee_qty") ? JsonValue["network_fee_qty"].asDouble() : throw std::runtime_error("field 'network_fee_qty' is not exist in json request");
  }
  catch (const std::runtime_error &e)
  {
    throw std::runtime_error(e.what());
  }
  return depositWithdrawal;
}

void ApiServer::Insert_depositWithdrawal(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Insert_depositWithdrawal\",\"id\":1,\"params\":{\"depositWithdrawal_obj\":{\"account_id\":46,\"coin_name\":\"BTC\",\"quantity_request\":1.0,\"commission_quantity\":0.0,\"type\":\"D\",\"time_request\":\"2023-04-06 07:16:13\",\"network_fee_qty\":0.0}}}" 144.217.82.52:8383

  try
  {
    Json::Value depositWithdrawal_obj = request["depositWithdrawal_obj"];
    DepositWithdrawal depositWithdrawal = JsonValue_to_depositWithdrawal(depositWithdrawal_obj);

    if (Database_manager::depositWithdrawal_dbt.Insert(depositWithdrawal))
    {
      int counter = 100;
      while (--counter > 0)
      {
        if (GlobalVariables::depositWithdrawals.find(depositWithdrawal.depositWithdrawal_id) != GlobalVariables::depositWithdrawals.end())
        {
          if (GlobalVariables::depositWithdrawals[depositWithdrawal.depositWithdrawal_id].processed)
          {

            if (GlobalVariables::depositWithdrawals[depositWithdrawal.depositWithdrawal_id].status == status_enum::Confirm)
            {
              response["status"] = "Confirmed";
              response["quantity_final"] = GlobalVariables::depositWithdrawals[depositWithdrawal.depositWithdrawal_id].quantity_final;
              response["depositWithdrawal_id"] = depositWithdrawal.depositWithdrawal_id;
            }
            else
            {
              response["status"] = "Rejected";
              response["reject_reason_id"] = GlobalVariables::depositWithdrawals[depositWithdrawal.depositWithdrawal_id].reject_reason_id;
            }

            unique_lock lock(GlobalVariables::mut_depositWithdrawals);
            GlobalVariables::depositWithdrawals.erase(depositWithdrawal.depositWithdrawal_id);

            break;
          }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
      }

      if (counter == 0)
        response["Error"] = "deposit/Withdrawal request not done";
    }
    else
    {
      response["Error"] = "Insert deposit/Withdrawal request into database failed";
    }
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}
//***************************************************************************************************

Json::Value ApiServer::CoinWalletBalance_to_JsonValue(int account_id, int coin_id)
{
  Json::Value response;
  try
  {
    if (coin_id == 0)
    {
      if (GlobalVariables::accounts.find(account_id) != GlobalVariables::accounts.end())
      {
        response["quantity"] = GlobalVariables::accounts[account_id].balance_usdt;
        response["coin_name"] = "USDT";
      }
      else
      {
        response["Error"] = "account is not exist";
      }
    }
    else
    {
      if (GlobalVariables::coins[coin_id].wallets.find(account_id) != GlobalVariables::coins[coin_id].wallets.end())
      {
        response["quantity"] = GlobalVariables::coins[coin_id].wallets[account_id].quantity;
      }
      else
      {
        response["quantity"] = 0.0;
      }
      response["coin_name"] = GlobalVariables::coins[coin_id].name;
    }
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }

  return response;
}

void ApiServer::Select_wallet_balance(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_wallet_balance\",\"id\":1,\"params\":{\"coin_name\":\"BTC\",\"account_id\":46}}" 144.217.82.52:8383
  try
  {
    int coin_id = CoinName_to_coinId(request["coin_name"].asString());
    int account_id = request["account_id"].asInt();

    response["account_id"] = account_id;
    response[request["coin_name"].asString()] = CoinWalletBalance_to_JsonValue(account_id, coin_id);
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

void ApiServer::Select_wallet_balance_all_coins(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_wallet_balance_all_coins\",\"id\":1,\"params\":{\"account_id\":46}}" 144.217.82.52:8383
  try
  {
    int account_id = request["account_id"].asInt();

    response["account_id"] = account_id;
    response["USDT"] = CoinWalletBalance_to_JsonValue(account_id, 0);

    if (GlobalVariables::coins.size() > 0)
    {
      for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
      {
        response[it_coin->second.name] = CoinWalletBalance_to_JsonValue(account_id, it_coin->second.coin_id);
      }
    }
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

//***************************************************************************************************

Coin ApiServer::JsonValue_to_Coin(const Json::Value &JsonValue)
{
  Coin coin;

  try
  {
    coin.coin_id = JsonValue.isMember("coin_name") ? CoinName_to_coinId(JsonValue["coin_name"].asString()) : throw std::runtime_error("field 'coin_name' is not exist in json request");
    coin.name = JsonValue.isMember("name") ? JsonValue["name"].asString() : throw std::runtime_error("name");
    coin.riskMargin = JsonValue.isMember("riskMargin") ? JsonValue["riskMargin"].asDouble() : throw std::runtime_error("riskMargin");
    coin.lockMargin = JsonValue.isMember("lockMargin") ? JsonValue["lockMargin"].asDouble() : throw std::runtime_error("lockMargin");
    coin.quantity_precision = JsonValue.isMember("quantity_precision") ? JsonValue["quantity_precision"].asInt() : throw std::runtime_error("quantity_precision");
    coin.price_decimal = JsonValue.isMember("price_decimal") ? JsonValue["price_decimal"].asInt() : throw std::runtime_error("price_decimal");
    coin.quantity_decimal = JsonValue.isMember("quantity_decimal") ? JsonValue["quantity_decimal"].asInt() : throw std::runtime_error("quantity_decimal");
    coin.encouragement_perc = JsonValue.isMember("encouragement_perc") ? JsonValue["encouragement_perc"].asDouble() : throw std::runtime_error("encouragement_perc");
    coin.tradeRequestGenerator_id = JsonValue.isMember("tradeRequestGenerator_id") ? JsonValue["tradeRequestGenerator_id"].asInt() : throw std::runtime_error("tradeRequestGenerator_id");
    coin.maxLimitFeedStop_sec = JsonValue.isMember("maxLimitFeedStop_sec") ? JsonValue["maxLimitFeedStop_sec"].asInt() : throw std::runtime_error("maxLimitFeedStop_sec");
    coin.market_id = JsonValue.isMember("market_id") ? JsonValue["market_id"].asInt() : throw std::runtime_error("market_id");
    coin.loopSleep_ms = JsonValue.isMember("loopSleep_ms") ? JsonValue["loopSleep_ms"].asInt() : throw std::runtime_error("loopSleep_ms");
    coin.coinMinBalance_alertMessageThreshold_id = JsonValue.isMember("coinMinBalance_alertMessageThreshold_id") ? JsonValue["coinMinBalance_alertMessageThreshold_id"].asInt() : throw std::runtime_error("coinMinBalance_alertMessageThreshold_id");
  }
  catch (const std::runtime_error &e)
  {
    throw std::runtime_error(e.what());
  }

  return coin;
}

Json::Value ApiServer::Coin_to_jsonValue(const Coin &coin)
{
  Json::Value res;

  res["coin_name"] = coin.name;
  res["riskMargin"] = coin.riskMargin;
  res["lockMargin"] = coin.lockMargin;
  res["quantity_precision"] = coin.quantity_precision;
  res["price_decimal"] = coin.price_decimal;
  res["quantity_decimal"] = coin.quantity_decimal;
  res["encouragement_perc"] = coin.encouragement_perc;
  res["market_buy_price"] = coin.market_buy_price;
  res["market_sell_price"] = coin.market_sell_price;
  res["time_price"] = UtilityTools::Timestamp2DatetimeString(coin.time_price, false);
  res["tradeRequestGenerator_id"] = coin.tradeRequestGenerator_id;
  res["maxLimitFeedStop_sec"] = coin.maxLimitFeedStop_sec;
  res["market_id"] = coin.market_id;
  res["loopSleep_ms"] = coin.loopSleep_ms;
  res["coinMinBalance_alertMessageThreshold_id"] = coin.coinMinBalance_alertMessageThreshold_id;

  return res;
}

void ApiServer::Select_coin_all(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_coin_all\",\"id\":1,\"params\":{}}" 144.217.82.52:8383

  if (GlobalVariables::coins.size() > 0)
  {
    for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
    {
      response[it_coin->second.name] = Coin_to_jsonValue(GlobalVariables::coins[it_coin->second.coin_id]);
    }
  }
  else
  {
    response["Error"] = "there is no coin";
  }
}

void ApiServer::Select_coin(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_coin\",\"id\":1,\"params\":{\"coin_name\":\"BTC\"}}" 144.217.82.52:8383
  try
  {
    int coin_id = CoinName_to_coinId(request["coin_name"].asString());

    response["coin_obj"] = Coin_to_jsonValue(GlobalVariables::coins[coin_id]);
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

void ApiServer::Select_coin_price(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Select_coin_price\",\"id\":1,\"params\":{\"coin_name\":\"BTC\"}}" 144.217.82.52:8383
  try
  {
    int coin_id = CoinName_to_coinId(request["coin_name"].asString());

    response["coin_name"] = request["coin_name"];

    response["bid"] = GlobalVariables::coins[coin_id].market_buy_price;
    response["ask"] = GlobalVariables::coins[coin_id].market_sell_price;
    response["time"] = UtilityTools::Timestamp2DatetimeString(GlobalVariables::coins[coin_id].time_price, false);
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}

void ApiServer::Update_coin_price(const Json::Value &request, Json::Value &response)
{
  //  curl --data "{\"jsonrpc\":\"2.0\",\"method\":\"Update_coin_price\",\"id\":1,\"params\":{\"coin_name\":\"RIAL\",\"bid\":0.000051,\"ask\":0.000052}}" 144.217.82.52:8383
  try
  {
    int coin_id = CoinName_to_coinId(request["coin_name"].asString());

    GlobalVariables::coins[coin_id].market_buy_price = request["bid"].asDouble();
    GlobalVariables::coins[coin_id].market_sell_price = request["ask"].asDouble();
    GlobalVariables::coins[coin_id].time_price = time(0);

    response["status"] = "confirmed";
  }
  catch (const std::runtime_error &e)
  {
    response["Error"] = e.what();
  }
}
//***************************************************************************************************

unsigned ApiInterface::ThreadFunction()
{
  try
  {
    HttpServer server(8383);
    ApiServer serv(server);
    if (serv.StartListening())
    {
      Logger::logger.log_special(LogLocation, false, "Api Interface started successfully");
      while (isStopEventSignaled() == false)
      {
        this_thread::sleep_for(chrono::milliseconds(1000));
      }
      serv.StopListening();
    }
    else
    {
      Logger::logger.log_special(LogLocation, true, "Api Interface start failed");
    }
  }
  catch (jsonrpc::JsonRpcException &e)
  {
    Logger::logger.log_special(LogLocation, true, e.what());
  }

  return 0;
}

ApiInterface::ApiInterface()
{
}