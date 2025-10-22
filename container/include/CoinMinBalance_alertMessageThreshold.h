#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct CoinMinBalance_alertMessageThreshold
 * @brief Data structure representing minimum balance alert thresholds for coins
 * 
 * This structure contains threshold values for triggering alert messages
 * based on various trading conditions. It defines minimum balance requirements
 * for different trading scenarios and market conditions.
 * 
 * Key Features:
 * - Alert threshold identification
 * - Minimum trade price threshold
 * - Trade inactivity threshold
 * - Market wallet balance threshold
 * 
 * @note Contains alert threshold configuration
 * @note Defines minimum balance requirements for different scenarios
 * @note Supports multiple threshold types for comprehensive monitoring
 */
struct CoinMinBalance_alertMessageThreshold
{
    int coinMinBalance_alertMessageThreshold_id = 0;///< Unique identifier for the alert threshold
    double lessThanMinTradePrice = 0;///< Threshold for trades below minimum trade price
    double tradeIsNotActive = 0;///< Threshold for when trading is not active
    double marketWalletBalance = 0;///< Threshold for market wallet balance
};