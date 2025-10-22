#pragma once

#include "stdafx.h"
#include "Enums.h"
#include "Trade.h"
#include "Wallet.h"
#include "Prices.h"

class TradeRequestProcessor;
class PriceFeeder;

/**
 * @struct Coin
 * @brief Core data structure representing a cryptocurrency in the trading system
 * 
 * This structure encapsulates all information related to a specific cryptocurrency
 * including market data, trading parameters, wallet management, and associated
 * processors. It serves as the central data model for cryptocurrency operations
 * throughout the trading service.
 * 
 * Key Features:
 * - Cryptocurrency identification and metadata
 * - Real-time market price and quantity data
 * - Risk and margin management parameters
 * - Trading robot and generator associations
 * - Wallet management for multiple accounts
 * - Price feeder and processor references
 * - API precision settings for exchange compatibility
 * 
 * @note Contains both market data and trading configuration
 * @note Manages wallets for multiple accounts per coin
 * @note Includes references to price feeders and processors
 * @note Supports both USDT-based and other base currencies
 */
struct Coin
{
	int coin_id = 0;///< Unique identifier for the cryptocurrency
	string name = "";///< Name of the cryptocurrency (e.g., "BTC", "ETH")

	double riskMargin = 0;///< Risk margin percentage for trading calculations
	double lockMargin = 0;///< Lock margin percentage for position management
	int quantity_precision = 0;///< Decimal precision for quantity calculations

	double encouragement_perc = 0;///< Encouragement percentage for trading incentives. It's useless in OMS
	double market_buy_price = 0;  ///< Current market bid price
	double market_sell_price = 0; ///< Current market ask price
	double market_price_coefficient = 0;///< Market price coefficient for calculations

	double market_buy_quantity = 0;	 ///< Current market bid quantity
	double market_sell_quantity = 0; ///< Current market ask quantity

	time_t time_price = 0;///< Timestamp of the last price update
	int tradeRequestGenerator_id = 0;///< ID of the associated trade request generator
	int maxLimitFeedStop_sec;///< Maximum time limit for feed stop in seconds
	int loopSleep_ms = 0;///< Sleep time in milliseconds for processing loops
	int tradeRequestRobotID_buyer = 0;///< ID of the buyer trade request robot
	int tradeRequestRobotID_seller = 0;///< ID of the seller trade request robot

	map<int, Wallet> wallets; ///< Map of wallets indexed by account ID (account_id, wallet)
	int coinMinBalance_alertMessageThreshold_id = 0;///< ID of the minimum balance alert threshold

	TradeRequestProcessor *tradeRequestProcessor = 0;///< Pointer to the trade request processor
	PriceFeeder *priceFeeder = 0;///< Primary price feeder for market data
	PriceFeeder *priceFeeder2 = 0;///< Secondary price feeder for redundancy
	int market_id = 0;///< ID of the associated market/exchange
	int income_deposit_account_id = 0;///< ID of the income deposit account
	Prices previous_prices;///< Previous price data for comparison
	int lastTradePriceGenerator_id;///< ID of the last trade price generator
	bool is_usdtBase = true;///< Flag indicating if the coin is USDT-based

#pragma region
	int price_decimal = 0;///< Decimal precision for price values (API compatibility)
	int quantity_decimal = 0;///< Decimal precision for quantity values (API compatibility)
#pragma endregion < just added for API>
};