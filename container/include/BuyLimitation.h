#pragma once

#include "stdafx.h"
#include "Enums.h"


/**
 * @struct BuyLimitation
 * @brief Data structure representing buy limitation rules for trading
 * 
 * This structure contains buy limitation rules that restrict trading
 * activities based on seller, coin, price, and quantity constraints.
 * It helps manage risk and enforce trading limits within the system.
 * 
 * Key Features:
 * - Buy limitation identification and metadata
 * - Seller and coin-specific limitations
 * - Maximum sell price and buy quantity constraints
 * - Enable/disable functionality
 * - Commission rate association
 * 
 * @note Contains trading limitation rules and constraints
 * @note Supports seller and coin-specific limitations
 * @note Includes price and quantity restrictions
 */
struct BuyLimitation
{
   int buyLimitation_id=0;///< Unique identifier for the buy limitation rule
   int seller_id=0;///< ID of the seller account this limitation applies to
   int coin_id=0;///< ID of the cryptocurrency this limitation applies to
   double max_sell_price=0;///< Maximum sell price allowed for this limitation
   double max_buy_qty=0;///< Maximum buy quantity allowed for this limitation
   bool enable=false;///< Flag indicating if this limitation is enabled
   int commissionRate_id=0;///< ID of the commission rate to apply for this limitation
};