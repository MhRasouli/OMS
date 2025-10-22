#pragma once

//    https://github.com/cinemast/libjson-rpc-cpp/blob/master/src/examples/simpleserver.cpp

#include "stdafx.h"
#include "AThread.h"
#include "Database_manager.h"
#include "Enums.h"
#include "GlobalVariables.h"
#include "UtilityTools.h"
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "Database_manager.h"

using namespace jsonrpc;

class ApiServer : public AbstractServer<ApiServer>
{
private:
  int CoinName_to_coinId(string coinName);
  string CoinId_to_coinName(int coinId);

  TradeRequest JsonValue_to_tradeRequest(const Json::Value &JsonValue);
  Json::Value TradeRequest_to_jsonValue(TradeRequest tradeRequest);

  Json::Value Coin_to_jsonValue(const Coin &coin);
  Coin JsonValue_to_Coin(const Json::Value &JsonValue);

  DepositWithdrawal JsonValue_to_depositWithdrawal(const Json::Value &JsonValue);
  Json::Value CoinWalletBalance_to_JsonValue(int account_id,int coin_id);

public:
  ApiServer(HttpServer &server);

  void Insert_depositWithdrawal(const Json::Value &request, Json::Value &response);

  void Select_tradeRequests_account(const Json::Value &request, Json::Value &response);
  void Select_tradeRequest(const Json::Value &request, Json::Value &response);
  void Insert_tradeRequest(const Json::Value &request, Json::Value &response);
  void Update_tradeRequest(const Json::Value &request, Json::Value &response);

  void Select_wallet_balance(const Json::Value &request, Json::Value &response);
  void Select_wallet_balance_all_coins(const Json::Value &request, Json::Value &response);

  void Select_coin_all(const Json::Value &request, Json::Value &response);
  void Select_coin(const Json::Value &request, Json::Value &response);
  void Select_coin_price(const Json::Value &request, Json::Value &response);
  void Update_coin_price(const Json::Value &request, Json::Value &response);
};

class ApiInterface : public AThread
{
public:
  ApiInterface();

private:
  unsigned ThreadFunction();
};