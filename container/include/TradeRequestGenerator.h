#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct TradeRequestGenerator
 * @brief Data structure representing trade request generation configuration
 * 
 * This structure contains configuration parameters for automatically
 * generating trade requests. It defines price ladder settings, quantity
 * change probabilities, and price range constraints for automated trading.
 * 
 * Key Features:
 * - Trade request generator identification
 * - Price ladder distance configuration
 * - Quantity change probability settings
 * - Price range constraints (min/max)
 * - Queue length management
 * - Account association for generation
 * 
 * @note Contains automated trading configuration
 * @note Defines price ladder and quantity change parameters
 * @note Supports price range constraints and queue management
 */
struct TradeRequestGenerator
{
	int tradeRequestGenerator_id=0;///< Unique identifier for the trade request generator
	double priceLadderDistanceSizeMin_perc = 0;///< Minimum price ladder distance as percentage
	double priceLadderDistanceSizeMax_perc = 0;///< Maximum price ladder distance as percentage
	double quantityChangeProbability_perc = 0;///< Probability of quantity changes as percentage
	bool enable=false;///< Flag indicating if the generator is enabled
	int accountID_Generator=0;///< ID of the account used for generation
	int queueLength=0;///< Length of the generation queue
	int maxPrice_usd=0;///< Maximum price in USD for generated requests
	int minPrice_usd=0;///< Minimum price in USD for generated requests
};