#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct Prices
 * @brief Data structure representing price information for a cryptocurrency
 * 
 * This structure contains current market price data including bid/ask prices,
 * quantities, and last traded price information. It serves as the data model
 * for price snapshots and market data storage.
 * 
 * Key Features:
 * - Cryptocurrency identification
 * - Bid and ask price information
 * - Bid and ask quantity data
 * - Last traded price (LTP) tracking
 * - Price flag for data validation
 * 
 * @note Simple data structure for price information
 * @note Contains both price and quantity data
 * @note Includes last traded price with validation flag
 */
struct Prices
{
    int coin_id = 0;///< ID of the cryptocurrency
    double ask = 0;///< Current ask (sell) price
    double bid = 0;///< Current bid (buy) price
    double ask_qty = 0;///< Available quantity at ask price
    double bid_qty = 0;///< Available quantity at bid price
    double ltp = 0;///< Last traded price
    char ltp_flag='D';///< Flag for last traded price validation ('D' = default)
};