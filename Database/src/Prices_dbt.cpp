#include "Prices_dbt.h"
#include "Prices.h"

Prices_dbt::Prices_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "prices")
{
}

bool Prices_dbt::Update_bid_ask(const Prices prices)
{
    bool result = false;
    try
    {
        double ask = 0;
        double bid = 0;
        double ask_qty = 0;
        double bid_qty = 0;
        if (UtilityTools::Coin_is_UsdtBase(prices.coin_id))
        {
            ask = prices.ask;
            bid = prices.bid;
            ask_qty = prices.ask_qty;
            bid_qty = prices.bid_qty;
        }
        else
        {
            bid = UtilityTools::Get_price_DbRegion(prices.ask, prices.coin_id);
            ask = UtilityTools::Get_price_DbRegion(prices.bid, prices.coin_id);
            bid_qty = prices.ask_qty;
            ask_qty = prices.bid_qty;
        }

        result = UpdateTable("coin_id=" + to_string(prices.coin_id), "ask=%.8f,bid=%.8f,ask_qty=%.8f,bid_qty=%.8f", ask, bid, ask_qty, bid_qty);
    }
    CATCH_STATEMENT

    return result;
}
bool Prices_dbt::Update_ltp(const Prices prices)
{
    bool result = false;
    try
    {
        double ltp = 0;
        if (UtilityTools::Coin_is_UsdtBase(prices.coin_id))
        {
            ltp = prices.ltp;
        }
        else
        {
            ltp = UtilityTools::Get_price_DbRegion(prices.ltp, prices.coin_id);
        }

        result = UpdateTable("coin_id=" + to_string(prices.coin_id), "ltp=%.8f, ltp_flag='%c'", ltp, prices.ltp_flag);
    }
    CATCH_STATEMENT

    return result;
}

bool Prices_dbt::Update(const Prices prices)
{
    bool result = false;
    try
    {
        double ask = 0;
        double bid = 0;
        double ask_qty = 0;
        double bid_qty = 0;
        double ltp = 0;
        if (UtilityTools::Coin_is_UsdtBase(prices.coin_id))
        {
            ask = prices.ask;
            bid = prices.bid;
            ask_qty = prices.ask_qty;
            bid_qty = prices.bid_qty;
            ltp = prices.ltp;
        }
        else
        {
            bid = UtilityTools::Get_price_DbRegion(prices.ask, prices.coin_id);
            ask = UtilityTools::Get_price_DbRegion(prices.bid, prices.coin_id);
            bid_qty = prices.ask_qty;
            ask_qty = prices.bid_qty;

            ltp = UtilityTools::Get_price_DbRegion(prices.ltp, prices.coin_id);
        }

        result = UpdateTable("coin_id=" + to_string(prices.coin_id), "ask=%.8f,bid=%.8f,ask_qty=%.8f,bid_qty=%.8f, ltp=%.8f, ltp_flag='%c'", ask, bid, ask_qty, bid_qty, ltp, prices.ltp_flag);
    }
    CATCH_STATEMENT

    return result;
}
