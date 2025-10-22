#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct CommissionRateRule
 * @brief Data structure representing commission rate rules for different trade locations
 * 
 * This structure contains commission rate rules that apply different commission
 * rates based on the location where the trade is executed. It supports market,
 * broker, and local trade locations with separate commission rates for each.
 * 
 * Key Features:
 * - Commission rate rule identification
 * - Location-specific commission rate associations
 * - Market, broker, and local commission rates
 * - Commission swap percentage configuration
 * 
 * @note Contains location-based commission rate rules
 * @note Supports multiple trade execution locations
 * @note Includes commission swap percentage settings
 */
struct CommissionRateRule
{
    int commissionRateRule_id = 0;///< Unique identifier for the commission rate rule
    int commissionRate_id_market = 0;///< ID of the commission rate for market trades
    int commissionRate_id_broker = 0;///< ID of the commission rate for broker trades
    int commissionRate_id_local = 0;///< ID of the commission rate for local trades
    double commissionSwap_perc = 0.0;///< Commission swap percentage
};
