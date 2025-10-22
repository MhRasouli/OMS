#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "Configuration.h"
#include "Database_manager.h"
#include "UtilityTools.h"
#include "CircularArray.h"
#include "Change.h"
#include "Enums.h"
#include "GlobalVariables.h"
#include "BinanceTrader.h"
#include "MarketTrader.h"
#include "NobitexTrader.h"
#include "KucoinTrader.h"
#include "TradeResult.h"
#include "CommissionRate.h"

struct Coin;

class TradeRequestProcessor : public AThread
{
public:
	TradeRequestProcessor(Coin *_coin);
	map<int, TradeRequest> enteredTradeRequests;
	map<int, TradeRequest> tradeRequests; // map < TradeRequest_id, TradeRequest>

	shared_mutex mux;
	Coin *coin;
	bool startProccess = false;

private:
	MarketTrader *marketTrader;
	int robotActionDelay_buyer = 1;
	int robotActionDelay_seller = 1;

	int robotActionDelay_noOrder_buyer = 1;
	int robotActionDelay_noOrder_seller = 1;

	
	list<int> generatedTradeRequests_buy;  // map < coin_id, list<generatedTradeRequests_id>>
	list<int> generatedTradeRequests_sell; // map < coin_id, list<generatedTradeRequests_id>>

	list<int> robotSellerTRs_sell;
	list<int> robotBuyerTRs_buy;

	int64_t time_lastGottentime = 0;
	int64_t time_lastGenerated_ltp = 0;

	int mainLoopTimer = 0;
	int mainLoopCounter = 0;

	unsigned ThreadFunction();
	double get_minimumAllowedTradeAmount();
	void updateCoinPrices();
	bool CaculateTradeResult_buyer(TradeRequest *tradeRequest, TradeRequest &determinative_tradeRequest, TradeResult &tradeResult, tradeLocation_enum tradeLocation, double buyer_MarketTradePrice_usdt, double buyer_swap_tradePrice);
	bool CaculateTradeResult_seller(TradeRequest *tradeRequest, TradeRequest &determinative_tradeRequest, TradeResult &tradeResult, tradeLocation_enum tradeLocation, double seller_MarketTradePrice_usdt);

	int Trade_local(TradeRequest *tr_buyer, TradeRequest *tr_seller, TradeRequest &determinative_tradeRequest, tradeLocation_enum tradeLocation);
	int Trade_market_buy(TradeRequest *tr_buyer);
	int Trade_market_sell(TradeRequest *tr_seller);
	bool Check_IsTradingInMarketEnable(TradeRequest &tradeRequest);
	bool Get_tradeRequestGenerator_properties(double &priceLadderDistanceSizeMin_perc, double &priceLadderDistanceSizeMax_perc);

	bool Trade_request_is_waste(TradeRequest &tradeRequest);
	bool If_possible_do_trade_local(TradeRequest &tradeRequest_A, TradeRequest &tradeRequest_B, TradeRequest &determinative_tradeRequest);
	bool If_possible_do_trade_Market_buy(TradeRequest *tr_buyer);
	bool If_possible_do_trade_Market_sell(TradeRequest *tr_seller);
	TradeRequest Get_determinative_tradeRequest(TradeRequest tradeRequest_A, TradeRequest tradeRequest_B);

	bool IsBrokerAccount(int account_id);
	void Generate_tradeRequest_queue();
	void Printout_mainLoop_log();
	void Loop_delay();
	void Modify_generatedTradeRequest_queue();
	void check_tradeRequest(TradeRequest &tradeRequest);
	void Check_trade_requests();
	bool Check_do_local_trade(TradeRequest &tradeRequest);
	bool Check_do_market_trade(TradeRequest &tradeRequest);
	bool Check_stop_order(TradeRequest &tradeRequest);
	bool TpSlProc(Trade trade, const int tpsl_RefTrade_id);
	bool Check_BuyerLimitation(TradeRequest &tr_buyer, TradeRequest &tr_seller,TradeRequest &determinative_tradeRequest);
	map<int, TradeRequest>::reverse_iterator Get_NextTradeRequest(map<int, TradeRequest>::reverse_iterator it_tradeRequest);

	CommissionRate calc_buyer_commissionRate(int account_id, CommissionRate CR, double commission_perc);
	double calc_buyer_price_trade(double price_request, double coin_price, CommissionRate CR);
	double calc_buyer_price_execution(double price_request, double coin_price, CommissionRate CR);
	double calc_buyer_trade_quantity(double tradeQuantity, double totalTradePrice, double price_trade, double coin_price, CommissionRate CR, double account_balance_usdt);
	double calc_buyer_commission_qty(double tradeQuantity, double coin_price, CommissionRate CR);
	double calc_buyer_commission_usd(double tradeQuantity, double coin_price, CommissionRate CR);

	CommissionRate calc_seller_commissionRate(int account_id, CommissionRate CR);
	double calc_seller_trade_quantity(double tradeQuantity, int account_id);
	double calc_seller_price_trade(double price_request, double coin_price, CommissionRate CR);
	double calc_seller_price_execution(double price_request, double coin_price, CommissionRate CR);
	double calc_seller_commission_qty(double tradeQuantity, double coin_price, CommissionRate CR);
	double calc_seller_commission_usd(double tradeQuantity, double coin_price, CommissionRate CR);
	//*************************************************************************************************
	bool IsRobotAccount(int account_id);
	bool IsSamePriceExistInVector(double price, list<int> &robotTRs);
	void LogOutRobotTradeRequests();
	void LogOutInsertDeleteRobotTradeRequest(string str_type, int tradeRequest_id, int bestTradeRequestID_buy, int bestTradeRequestID_sell, bool calledBeforeFunction);
	void Initial_lstRobotBuyerTRs();
	void Initial_lstRobotSellerTRs();
	void Get_bestSecondTradeRequestID(side_enum tradeRequest_side, int &bestTradeRequestID, int &secondTradeRequestID);
	int Get_DistanceFromBestRequest(int tradeRequest_id, side_enum tradeRequest_side);
	void RobotSeller_TradeRequest_queue();
	void RobotBuyer_TradeRequest_queue();
	bool SyncRobotsBalance();
	void Generate_OneRobotTradeRequest_sell(TradeRequestRobot &robot, int trID_bestBuy, int trID_bestSell);
	void Generate_OneRobotTradeRequest_buy(TradeRequestRobot &robot, int trID_bestBuy, int trID_bestSell);
};