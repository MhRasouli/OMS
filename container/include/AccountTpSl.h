#pragma once

#include "stdafx.h"
#include "Enums.h"
#include "Account.h"
#include "Coin.h"
#include "Trade.h"
#include "TradeRequest.h"

/**
 * @struct TradeTpSl
 * @brief Data structure representing take profit/stop loss information for a trade
 * 
 * This structure contains take profit/stop loss (TP/SL) information for individual
 * trades, including remaining quantity, price, and associated coin.
 */
struct TradeTpSl
{
	int trade_id;///< ID of the trade
	double remained_qty;///< Remaining quantity for TP/SL
	double price;///< Price for TP/SL
	int coin_id;///< ID of the cryptocurrency
};

/**
 * @struct CoinTpSl
 * @brief Data structure representing take profit/stop loss information for a coin
 * 
 * This structure contains TP/SL information for a specific coin, including
 * initial quantity and lists of buy/sell TP/SL trades.
 */
struct CoinTpSl
{
	int coin_id;///< ID of the cryptocurrency
	double init_qty;///< Initial quantity for TP/SL calculations
	list<TradeTpSl> tradeTpSls_buy;///< List of buy TP/SL trades
	list<TradeTpSl> tradeTpSls_sell;///< List of sell TP/SL trades
};

/**
 * @struct AccountTpSl
 * @brief Data structure representing take profit/stop loss information for an account
 * 
 * This structure contains TP/SL information for a specific account, including
 * coin-specific TP/SL data organized by coin ID.
 */
struct AccountTpSl
{
	int account_id;///< ID of the account
	map<int, CoinTpSl> coinTpSls;///< Map of coin TP/SL data indexed by coin ID
};