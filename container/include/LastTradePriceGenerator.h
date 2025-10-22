#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct LastTradePriceGenerator
 * @brief Data structure representing last trade price generation configuration
 * 
 * This structure contains configuration parameters for generating last trade
 * prices based on current market conditions. It defines price area calculations
 * and generation timing for automated price generation.
 * 
 * Key Features:
 * - Last trade price generator identification
 * - Price area calculation around ask/bid average
 * - Generation delay timing controls
 * - Percentage-based price area configuration
 * 
 * @note Contains automated price generation configuration
 * @note Defines price area calculations around market average
 * @note Supports configurable generation delays
 */
struct LastTradePriceGenerator
{
    int lastTradePriceGenerator_id = 0;///< Unique identifier for the last trade price generator
    double priceAreaAroundAvgOfAskBid_min_perc = 0;///< Minimum price area around ask/bid average as percentage
    double priceAreaAroundAvgOfAskBid_max_perc = 0;///< Maximum price area around ask/bid average as percentage
    double generateDelay_min_sec = 0;///< Minimum generation delay in seconds
    double generateDelay_max_sec = 0;///< Maximum generation delay in seconds
};