#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct CommissionRate
 * @brief Data structure representing commission rate configuration for trading
 * 
 * This structure contains commission rate settings for both buyers and sellers
 * in trading operations. It defines percentage-based commissions for prices,
 * quantities, and overall trades, along with minimum commission thresholds.
 * 
 * Key Features:
 * - Commission rate identification
 * - Buyer and seller commission percentages
 * - Price and quantity commission rates
 * - Minimum commission thresholds in USDT
 * - Separate configuration for buyers and sellers
 * 
 * @note Contains comprehensive commission rate configuration
 * @note Supports both percentage-based and minimum commission settings
 * @note Separate rates for buyers and sellers
 */
struct CommissionRate
{
    int commissionRate_id = 0;///< Unique identifier for the commission rate
    double buyer_price_perc = 0;///< Commission percentage for buyer price
    double buyer_quantity_perc = 0;///< Commission percentage for buyer quantity
    double buyer_commission_perc = 0;///< Overall commission percentage for buyer
    double seller_price_perc = 0;///< Commission percentage for seller price
    double seller_quantity_perc = 0;///< Commission percentage for seller quantity
    double seller_commission_perc = 0;///< Overall commission percentage for seller
    double buyer_min_commission_usdt=0;///< Minimum commission in USDT for buyer
    double seller_min_commission_usdt=0;///< Minimum commission in USDT for seller
};
