#pragma once

#include "stdafx.h"
#include "Enums.h"


/**
 * @struct Market
 * @brief Data structure representing a cryptocurrency exchange/market configuration
 * 
 * This structure contains all configuration information for connecting to and
 * trading on a specific cryptocurrency exchange. It includes connection details,
 * authentication credentials, trading parameters, and operational settings.
 * 
 * Key Features:
 * - Market identification and metadata
 * - Connection configuration (hosts, ports)
 * - Authentication credentials (API keys, passphrases)
 * - Trading status and validation settings
 * - Minimum trade price and account routing
 * - Wallet balance monitoring configuration
 * 
 * @note Contains complete exchange configuration
 * @note Supports multiple connection endpoints (trade, price feed)
 * @note Includes security credentials and operational parameters
 * @note Manages trading status and validation rules
 */
struct Market
{
   int market_id=0;///< Unique identifier for the market/exchange
   string name="";///< Name of the market/exchange (e.g., "Binance", "Kucoin")
   bool enable=false;///< Flag indicating if this market is enabled
   string host_trade="";///< Hostname for trading API connection
   int port_trade=0;///< Port number for trading API connection
   string host_feedPrice="";///< Hostname for price feed connection
   int port_feedPrice=0;///< Port number for price feed connection
   string username_privateKey="";///< Username or private key for API authentication
   string password_securityKey="";///< Password or security key for API authentication
   string passphrase="";///< Passphrase for API authentication (if required)
   bool tradeIsActive=false;///< Flag indicating if trading is currently active on this market
   int feedPricesIsValidUntil_sec=0;///< Time in seconds until price feed data is considered invalid
   double minimumTradePrice_usd=0.0;///< Minimum trade price in USD for this market
   int accountID_TradeWith_LessThanMinTradePrice=0;///< Account ID to use for trades below minimum price
   int accountID_TradeWith_TradeIsNotActive=0;///< Account ID to use when trading is not active
   int accountID_MarketWalletBalance=0;///< Account ID for monitoring market wallet balance
   int checkMarketWalletBalanceEvery_sec=0;///< Interval in seconds for checking market wallet balance
};