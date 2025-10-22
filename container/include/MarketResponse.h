#pragma once

#include "stdafx.h"
#include "Enums.h"
#include "MarketResponseDetail.h"

/**
 * @struct MarketResponse
 * @brief Data structure representing market response from exchange API
 * 
 * This structure contains the response data received from exchange APIs
 * after submitting orders. It includes order details, execution information,
 * and commission data from the market response.
 * 
 * Key Features:
 * - Market response identification and metadata
 * - Order details (ID, symbol, client order ID)
 * - Execution information (price, quantities, status)
 * - Commission calculation method
 * - Detailed market response information
 * 
 * @note Contains comprehensive market response data
 * @note Includes commission calculation functionality
 * @note Links to detailed market response information
 */
struct MarketResponse
{
	int marketResponse_id = 0;///< Unique identifier for the market response
	string symbol="";///< Trading pair symbol (e.g., "BTCUSDT")
	string orderId = "";///< Exchange-assigned order ID
	int orderListId = 0;///< Order list ID (for batch orders)
	string clientOrderId;///< Client-assigned order ID
	time_t transactTime = 0;///< Transaction timestamp
	double price = 0;///< Order price
	double origQty = 0;///< Original order quantity
	double executedQty = 0;///< Executed quantity
	double cummulativeQuoteQty = 0;///< Cumulative quote quantity
	string status = "";///< Order status
	string timeInForce = "";///< Time in force setting
	string type = "";///< Order type
	string side = "";///< Order side (buy/sell)
	vector<MarketResponseDetail> lst_marketResponseDetail;///< List of detailed market response information

	/**
	 * @brief Calculates the total market commission from response details
	 * 
	 * Iterates through all market response details and sums up the
	 * commission amounts to get the total commission for this market response.
	 * 
	 * @return double Total commission amount
	 * 
	 * @note Sums commission from all market response details
	 * @note Returns 0 if no details are present
	 */
	double GetMarketCommission()
	{
		double res = 0;

		for (vector<MarketResponseDetail>::iterator it = lst_marketResponseDetail.begin();
			it != lst_marketResponseDetail.end(); it++)
		{
			res += it->commission;
		}

		return res;
	}
};