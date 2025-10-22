#include "TradeRequest.h"
#include "Coin.h"
#include "CommissionRate.h"
#include "GlobalVariables.h"
#include "UtilityTools.h"

bool TradeRequest::Get_CommissionCoeff_BasedOnStair(int account_id, double &result)
{
    bool res = false;
    try
    {
        for (StairCommission strComm : GlobalVariables::stairCommissions)
        {
            if ((this->type == type_enum::Market || this->type == type_enum::Swap))
            {
                result = strComm.marketOrder_commission_perc;
            }
            else if (this->type == type_enum::Limit || this->type == type_enum::Stop)
            {
                result = strComm.pendingOrder_commission_perc;
            }

            if (GlobalVariables::accounts[account_id].tradePrice_lastMonth_usdt < strComm.maxLastMonthTradePrice_usdt)
            {
                break;
            }
        }

        res = true;
    }
    CATCH_STATEMENT

    return res;
}

CommissionRate TradeRequest::calc_seller_commissionRate(int account_id, CommissionRate CR)
{
    CommissionRate comRate;
    try
    {
        double commissionCoeff_BasedOnStair;
        Get_CommissionCoeff_BasedOnStair(account_id, commissionCoeff_BasedOnStair);

        comRate = CR;
        comRate.seller_commission_perc *= commissionCoeff_BasedOnStair;
        comRate.seller_quantity_perc *= commissionCoeff_BasedOnStair;
    }
    CATCH_STATEMENT

    return comRate;
}

CommissionRate TradeRequest::calc_buyer_commissionRate(int account_id, CommissionRate CR, double commission_perc = 1.0)
{
    CommissionRate comRate;
    try
    {
        double commissionCoeff_BasedOnStair;
        Get_CommissionCoeff_BasedOnStair(account_id, commissionCoeff_BasedOnStair);

        comRate = CR;
        comRate.buyer_commission_perc *= commission_perc;
        comRate.buyer_commission_perc *= commissionCoeff_BasedOnStair;
    }
    CATCH_STATEMENT

    return comRate;
}

bool TradeRequest::Check_hasCommissionRule(tradeLocation_enum tradeLocation)
{
    CommissionRate commissionRate;
    return Get_CommissionRate(commissionRate, tradeLocation);
}

bool TradeRequest::Get_CommissionRate(CommissionRate &commissionRate, tradeLocation_enum tradeLocation)
{
    bool result = false;
    if (GlobalVariables::GroupCoinMarketCommissions.find(TwoKey(GlobalVariables::accounts[account_id].group_id, coin_id)) != GlobalVariables::GroupCoinMarketCommissions.end())
    {
        GroupCoinMarketCommission GCMC = GlobalVariables::GroupCoinMarketCommissions[TwoKey(GlobalVariables::accounts[account_id].group_id, coin_id)];
        //**********************************************************************************************
        CommissionRateRule commissionRateRule;
        if ((this->type == type_enum::Market || this->type == type_enum::Swap))
        {
            if (GCMC.marketOrder_commissionRateRule_id > 0)
                if (GlobalVariables::commissionRateRules.find(GCMC.marketOrder_commissionRateRule_id) != GlobalVariables::commissionRateRules.end())
                    commissionRateRule = GlobalVariables::commissionRateRules[GCMC.marketOrder_commissionRateRule_id];
        }
        else if (this->type == type_enum::Limit || this->type == type_enum::Stop)
        {
            if (GCMC.pendingOrder_commissionRateRule_id > 0)
                if (GlobalVariables::commissionRateRules.find(GCMC.pendingOrder_commissionRateRule_id) != GlobalVariables::commissionRateRules.end())
                    commissionRateRule = GlobalVariables::commissionRateRules[GCMC.pendingOrder_commissionRateRule_id];
        }

        if (commissionRateRule.commissionRateRule_id > 0)
        {
            if (tradeLocation == tradeLocation_enum::Broker)
            {
                if (GlobalVariables::commissionRates.find(commissionRateRule.commissionRate_id_broker) != GlobalVariables::commissionRates.end())
                    commissionRate = GlobalVariables::commissionRates[commissionRateRule.commissionRate_id_broker];
            }
            else if (tradeLocation == tradeLocation_enum::Market)
            {
                if (GlobalVariables::commissionRates.find(commissionRateRule.commissionRate_id_market) != GlobalVariables::commissionRates.end())
                    commissionRate = GlobalVariables::commissionRates[commissionRateRule.commissionRate_id_market];
            }
            else if (tradeLocation == tradeLocation_enum::Local)
            {
                if (GlobalVariables::commissionRates.find(commissionRateRule.commissionRate_id_local) != GlobalVariables::commissionRates.end())
                    commissionRate = GlobalVariables::commissionRates[commissionRateRule.commissionRate_id_local];
            }
        }
        if (commissionRate.commissionRate_id > 0)
        {
            if (this->get_side_OmsRegion() == side_enum::Buy)
            {
                if (this->type == type_enum::Swap)
                    commissionRate = calc_buyer_commissionRate(this->account_id, commissionRate, commissionRateRule.commissionSwap_perc);
                else
                    commissionRate = calc_buyer_commissionRate(this->account_id, commissionRate);
            }
            else
            {
                commissionRate = calc_seller_commissionRate(this->account_id, commissionRate);
            }

            result = true;
        }
    }

    if (!result)
    {
        string errorMessage = "there is no GroupCoinMarketCommission or CommissionRateRule or CommissionRate for tradeRequest_id:";
        errorMessage += to_string(tradeRequest_id);
        errorMessage += " ,group_id:";
        errorMessage += to_string(GlobalVariables::accounts[account_id].group_id);
        errorMessage += " ,coin_id:";
        errorMessage += to_string(coin_id);

        Logger::logger.log_special(LogLocation, true, errorMessage.c_str());

        throw runtime_error(errorMessage);
    }
    return result;
}

bool TradeRequest::Lock_order()
{
    bool result = false;
    if (GlobalVariables::coins[coin_id].market_id != 1 && !isLock)
    {
        isLock = true;
        result = true;
    }

    return result;
}

//******************************************************************************************* */
void TradeRequest::set_quantity_DbRegion(double quantity_DbRegion)
{
    m_quantity_DbRegion = quantity_DbRegion;
}
void TradeRequest::set_price_DbRegion(double price_DbRegion)
{
    m_price_DbRegion = price_DbRegion;
}
void TradeRequest::set_side_DbRegion(side_enum side_DbRegion)
{
    m_side_DbRegion = side_DbRegion;
}

double TradeRequest::get_quantity_DbRegion()
{
    return m_quantity_DbRegion;
}
double TradeRequest::get_price_DbRegion()
{
    return m_price_DbRegion;
}
side_enum TradeRequest::get_side_DbRegion()
{
    return m_side_DbRegion;
}
//******************************************************************************************* */
void TradeRequest::set_new_quantity_OmsRegion(double quantity_OmsRegion, double price_OmsRegion)
{
    if (price_OmsRegion != 0)
    {
        m_price_DbRegion = UtilityTools::Get_price_DbRegion(price_OmsRegion, coin_id);
        m_quantity_DbRegion = UtilityTools::Get_quantity_DbRegion(quantity_OmsRegion, price_OmsRegion, coin_id);
    }
    else
    {
        price_OmsRegion = UtilityTools::Get_price_OmsRegion(m_price_DbRegion, coin_id);
        m_quantity_DbRegion = UtilityTools::Get_quantity_DbRegion(quantity_OmsRegion, price_OmsRegion, coin_id);
    }
}
void TradeRequest::set_new_price_OmsRegion(double price_OmsRegion)
{
    m_price_DbRegion = UtilityTools::Get_price_DbRegion(price_OmsRegion, coin_id);
}
//******************************************************************************************* */
double TradeRequest::get_quantity_OmsRegion(double price_OmsRegion)
{
    if (!GlobalVariables::coins[coin_id].is_usdtBase && m_price_DbRegion == 0)
    {
        if (price_OmsRegion != 0)
            return UtilityTools::Get_quantity_OmsRegion(m_quantity_DbRegion, UtilityTools::Get_price_DbRegion(price_OmsRegion, coin_id), coin_id);
        else
            return 0;
    }
    return UtilityTools::Get_quantity_OmsRegion(m_quantity_DbRegion, m_price_DbRegion, coin_id);
}

double TradeRequest::get_price_OmsRegion()
{
    return UtilityTools::Get_price_OmsRegion(m_price_DbRegion, coin_id);
}
side_enum TradeRequest::get_side_OmsRegion()
{
    return UtilityTools::Get_side_OmsRegion(m_side_DbRegion, coin_id);
}
