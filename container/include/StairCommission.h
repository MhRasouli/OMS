#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct StairCommission
 * @brief Data structure representing stair commission configuration
 * 
 * This structure contains commission rate settings that vary based on
 * trading volume thresholds. It implements a tiered commission structure
 * where commission rates change based on monthly trading volume.
 * 
 * Key Features:
 * - Stair commission identification
 * - Market order commission percentage
 * - Pending order commission percentage
 * - Monthly trading volume threshold
 * 
 * @note Implements tiered commission structure
 * @note Separate rates for market and pending orders
 * @note Based on monthly trading volume thresholds
 */
struct StairCommission
{
    int stairCommission_id = 0;///< Unique identifier for the stair commission rule
    double marketOrder_commission_perc = 0;///< Commission percentage for market orders
    double pendingOrder_commission_perc = 0;///< Commission percentage for pending orders
    double maxLastMonthTradePrice_usdt = 0;///< Maximum monthly trading volume in USDT for this tier
};