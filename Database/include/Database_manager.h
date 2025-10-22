#pragma once

#include "Configuration.h"
#include "DatabaseTable.h"
#include "Trade_dbt.h"
#include "TradeRequest_dbt.h"
#include "Account_dbt.h"
#include "Change_dbt.h"
#include "Coin_dbt.h"
#include "Commission_dbt.h"
#include "MarketResponse_dbt.h"
#include "MarketResponseDetail_dbt.h"
#include "Wallet_dbt.h"
#include "WalletHistory_dbt.h"
#include "GlobalVariables.h"
#include "DatabaseHandler.h"
#include "TradeRequestGenerator.h"
#include "TradeRequestGenerator_dbt.h"
#include "Market_dbt.h"
#include "Message_dbt.h"
#include "CommissionRate_dbt.h"
#include "StairCommission_dbt.h"
#include "DepositWithdrawal_dbt.h"
#include "CoinMinBalance_alertMessageThreshold_dbt.h"
#include "Group_dbt.h"
#include "BuyLimitation_dbt.h"
#include "GroupCoinMarketCommission_dbt.h"
#include "TradeRequestRobot_dbt.h"
#include "Prices_dbt.h"
#include "LastTradePriceGenerator_dbt.h"
#include "CommissionRateRule_dbt.h"


/**
 * @class Database_manager
 * @brief Central database management class for the trading service
 * 
 * This class provides centralized database management functionality including
 * connection handling, initialization, and access to all database table objects.
 * It serves as the main interface for all database operations in the trading system.
 * 
 * Features:
 * - Database connection management and initialization
 * - Static access to all database table objects
 * - Connection lifecycle management (open, close)
 * - Centralized database configuration
 * 
 * @note All members are static for global access
 * @note Manages database connection lifecycle
 * @note Provides access to all database table objects
 */
class Database_manager
{
private:
	static DatabaseHandler databaseHandler;///< Database handler for connection management
	static sql::Driver *driver;///< SQL driver for database connectivity
	static sql::Connection *con;///< SQL connection object
	static bool Open_Database();///< Opens database connection

public:
	/**
	 * @brief Initializes the database manager and establishes connection
	 * 
	 * @return bool True if initialization was successful, false otherwise
	 * 
	 * @note Must be called at application startup
	 * @note Establishes database connection and initializes all table objects
	 */
	static bool Initialize();
	
	/**
	 * @brief Closes the database connection and cleans up resources
	 * 
	 * @return bool True if closure was successful, false otherwise
	 * 
	 * @note Should be called at application shutdown
	 * @note Closes database connection and cleans up resources
	 */
	static bool Close_Database();

	static Trade_dbt trade_dbt;///< Trade database table object
	static TradeRequest_dbt tradeRequest_dbt;///< Trade request database table object
	static Account_dbt account_dbt;///< Account database table object
	static Change_dbt change_dbt;///< Change database table object
	static Coin_dbt coin_dbt;///< Coin database table object
	static Commission_dbt commission_dbt;///< Commission database table object
	static MarketResponse_dbt marketResponse_dbt;///< Market response database table object
	static MarketResponseDetail_dbt marketResponseDetail_dbt;///< Market response detail database table object
	static Wallet_dbt wallet_dbt;///< Wallet database table object
	static WalletHistory_dbt walletHistory_dbt;///< Wallet history database table object
	static TradeRequestGenerator_dbt tradeRequestGenerator_dbt;///< Trade request generator database table object
	static Market_dbt market_dbt;///< Market database table object
	static Message_dbt message_dbt;///< Message database table object
	static CommissionRate_dbt commissionRate_dbt;///< Commission rate database table object
	static StairCommission_dbt stairCommission_dbt;///< Stair commission database table object
	static DepositWithdrawal_dbt depositWithdrawal_dbt;///< Deposit withdrawal database table object
	static CoinMinBalance_alertMessageThreshold_dbt coinMinBalance_alertMessageThreshold_dbt;///< Coin minimum balance alert threshold database table object
	static Group_dbt group_dbt;///< Group database table object
	static BuyLimitation_dbt buyLimitation_dbt;///< Buy limitation database table object
	static GroupCoinMarketCommission_dbt groupCoinMarketCommission_dbt;///< Group coin market commission database table object
	static TradeRequestRobot_dbt tradeRequestRobot_dbt;///< Trade request robot database table object
	static Prices_dbt prices_dbt;///< Prices database table object
	static LastTradePriceGenerator_dbt lastTradePriceGenerator_dbt;///< Last trade price generator database table object
	static CommissionRateRule_dbt commissionRateRule_dbt;///< Commission rate rule database table object
};
