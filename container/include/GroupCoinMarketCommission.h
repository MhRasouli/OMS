#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct GroupCoinMarketCommission
 * @brief Data structure representing commission rules for specific group-coin-market combinations
 * 
 * This structure contains commission rate rules that apply to specific combinations
 * of groups, coins, and markets. It allows for fine-grained commission control
 * based on the trading context and participant characteristics.
 * 
 * Key Features:
 * - Group-coin-market commission identification
 * - Group and coin associations
 * - Market order commission rate rule
 * - Pending order commission rate rule
 * 
 * @note Contains context-specific commission rate rules
 * @note Links groups, coins, and commission rate rules
 * @note Supports separate rules for market and pending orders
 */
struct GroupCoinMarketCommission
{
    int groupCoinMarketCommission_id = 0;///< Unique identifier for the group-coin-market commission rule
    int group_id = 0;///< ID of the group this commission rule applies to
    int coin_id = 0;///< ID of the coin this commission rule applies to
    int marketOrder_commissionRateRule_id= 0;///< ID of the commission rate rule for market orders
    int pendingOrder_commissionRateRule_id = 0;///< ID of the commission rate rule for pending orders
};