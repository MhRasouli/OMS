#pragma once

#include "stdafx.h"
#include "CommissionRate.h"

/**
 * @struct TradeResult
 * @brief Data structure representing the result of a trade execution
 * 
 * This structure contains comprehensive information about the result
 * of a trade execution including prices, deposits, commissions,
 * and commission rate details.
 * 
 * Key Features:
 * - Trade execution and trade price information
 * - Deposit quantity and USDT amounts
 * - Commission calculations (USD and quantity)
 * - Commission rate configuration
 * 
 * @note Contains comprehensive trade execution results
 * @note Includes both price and commission information
 * @note Links to commission rate configuration
 */
struct TradeResult
{
    double price_execution=0;///< Actual execution price of the trade
    double price_trade=0;///< Trade price used for calculations
    double deposit_quantity=0;///< Deposit quantity amount
    double deposit_usdt=0;///< Deposit amount in USDT
    double commission_usd=0;///< Commission amount in USD
    double commission_usd_price=0;///< Commission amount in USD for price component
    double commission_qty=0;///< Commission quantity amount
    CommissionRate commissionRate;///< Commission rate configuration used for this trade
};