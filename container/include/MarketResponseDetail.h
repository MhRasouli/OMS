#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct MarketResponseDetail
 * @brief Data structure representing detailed market response information
 * 
 * This structure contains detailed information about individual trades
 * within a market response, including price, quantity, commission,
 * and trade identification.
 * 
 * Key Features:
 * - Market response detail identification
 * - Trade execution details (price, quantity)
 * - Commission information and asset
 * - Trade ID association
 * 
 * @note Contains detailed trade execution information
 * @note Links to parent market response
 * @note Includes commission and trade identification
 */
struct MarketResponseDetail
{
	int marketResponseDetail_id=0;///< Unique identifier for the market response detail
	int marketResponse_id = 0;///< ID of the parent market response
	double price = 0;///< Execution price for this detail
	double qty = 0;///< Executed quantity for this detail
	double commission = 0;///< Commission amount for this detail
	string commissionAsset = "";///< Asset used for commission payment
	int tradeId = 0;///< ID of the associated trade
};


