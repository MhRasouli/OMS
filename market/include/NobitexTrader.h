// NobitexTrader
#pragma once

#include "stdafx.h"
#include "Logger.h"
#include "UtilityTools.h"
#include "MarketTrader.h"
#include <cpr/cpr.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

class NobitexTrader : public MarketTrader
{
public:
    NobitexTrader(Coin *_coin);
    bool MarketOrder(int tradeRequest_id, side_enum side, double quantity, double commission_qty, double price, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason) override;
    bool CheckTradeDone(int orderId, int tradeRequest_id, side_enum side, double quantity, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason);
};
