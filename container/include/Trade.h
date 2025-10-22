#pragma once
#include "stdafx.h"

/**
 * @struct Trade
 * @brief Data structure representing a completed trade transaction
 * 
 * This structure contains all information related to a completed trade
 * including buyer and seller details, prices, quantities, and associated
 * trade requests. It serves as the data model for trade history and
 * transaction records in the trading system.
 * 
 * Key Features:
 * - Trade identification and metadata
 * - Buyer and seller account information
 * - Price and quantity details for both parties
 * - Commission and limitation tracking
 * - Take profit/Stop loss (TP/SL) management
 * - Market response and trade request associations
 * 
 * @note Represents a completed trade between two parties
 * @note Contains both buyer and seller perspective data
 * @note Tracks TP/SL settings and active status
 * @note Links to original trade requests and market responses
 */
struct Trade
{
	int trade_id = 0;///< Unique identifier for the trade
	int seller_id = 0;///< ID of the seller account
	int buyer_id = 0;///< ID of the buyer account
	int coin_id = 0;///< ID of the cryptocurrency being traded
	int commission_id = 0;///< ID of the commission applied to this trade
	int tradeRequestSeller_id = 0;///< ID of the seller's trade request
	int tradeRequestBuyer_id = 0;///< ID of the buyer's trade request
	int marketResponse_id = 0;///< ID of the market response for this trade
	int buyLimitation_id = 0;///< ID of the buy limitation applied
	double price_seller = 0;///< Price at which the seller executed the trade
	double quantity_seller = 0;///< Quantity sold by the seller
	double tpSl_seller_maxQty = 0;///< Maximum quantity for seller's TP/SL
	double price_buyer = 0;///< Price at which the buyer executed the trade
	double quantity_buyer = 0;///< Quantity bought by the buyer
	double tpSl_buyer_maxQty = 0;///< Maximum quantity for buyer's TP/SL
	time_t time = 0;///< Timestamp when the trade was executed
	bool tpSl_seller_active = false;///< Flag indicating if seller's TP/SL is active
	bool tpSl_buyer_active = false;///< Flag indicating if buyer's TP/SL is active
};
