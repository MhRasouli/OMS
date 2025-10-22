#pragma once

#include "stdafx.h"
#include "Coin.h"
#include "Account.h"
#include "Wallet.h"
#include "TradeRequest.h"
#include "Change.h"
#include "PriceFeederManager.h"
#include "TradeRequestGenerator.h"
#include "Market.h"
#include "Trade.h"
#include "AccountTpSl.h"
#include "CommissionRate.h"
#include "StairCommission.h"
#include "DepositWithdrawal.h"
#include "CoinMinBalance_alertMessageThreshold.h"
#include "Group.h"
#include "BuyLimitation.h"
#include "GroupCoinMarketCommission.h"
#include "TradeRequestRobot.h"
#include "LastTradePriceGenerator.h"
#include "Enums.h"
#include "CommissionRateRule.h"
#include "MarketObjects.h"

class PriceFeederManager;
struct MarketObjects;

/**
 * @namespace GlobalVariables
 * @brief Global variables namespace for the trading service
 * 
 * This namespace contains all global variables used throughout the trading service.
 * It provides centralized access to core data structures, managers, and configuration
 * objects that need to be shared across multiple components of the system.
 * 
 * Features:
 * - Core data structure maps (coins, markets, accounts, etc.)
 * - Thread-safe task queue for asynchronous operations
 * - Price feeder manager for market data coordination
 * - Commission and trading rule configurations
 * - Market-specific object management
 * - Python interface objects for external integrations
 * 
 * All variables are declared as extern to allow definition in separate source files
 * while providing global access throughout the application.
 * 
 * @note All variables are extern declarations - definitions are in corresponding .cpp files
 * @note Thread-safe access patterns are used where appropriate
 * @note Provides centralized data management for the entire trading system
 */
namespace GlobalVariables
{
	extern int MainLoopCounter;///< Counter for main application loop iterations

	extern std::queue<std::packaged_task<std::map<std::string, std::string>()>> taskQueue;///< Task queue for asynchronous operations
	extern std::mutex queueMutex;///< Mutex for thread-safe task queue access
	extern std::condition_variable cv;///< Condition variable for task queue synchronization
	extern py::object kucoinMarketInterface;///< Python object interface for Kucoin market operations

	extern map<int, Coin> coins;///< Map of coin objects indexed by coin ID
	extern map<int, Market> markets;///< Map of market objects indexed by market ID
	extern map<int, Account> accounts;///< Map of account objects indexed by account ID
	extern map<int, Change> changes;///< Map of change objects indexed by change ID
	extern PriceFeederManager priceFeederManager;///< Global price feeder manager instance
	extern map<int, TradeRequestGenerator> tradeRequestGenerators;///< Map of trade request generators indexed by generator ID
	extern map<int, AccountTpSl> accountTpSls; ///< Map of account take profit/stop loss objects indexed by account ID
	extern map<int, CommissionRate> commissionRates;///< Map of commission rates indexed by rate ID
	extern list<StairCommission> stairCommissions;///< List of stair commission configurations

	extern shared_mutex mut_depositWithdrawals;///< Shared mutex for thread-safe deposit/withdrawal access
	extern map<int, DepositWithdrawal> depositWithdrawals;///< Map of deposit/withdrawal objects indexed by ID

	extern map<int, CoinMinBalance_alertMessageThreshold> coinMinBalanceThresholds;///< Map of coin minimum balance alert thresholds
	extern map<int, Group> groups;///< Map of group objects indexed by group ID
	extern map<int, BuyLimitation> buyLimitations;///< Map of buy limitation rules indexed by limitation ID
	extern map<pair<int, int>, GroupCoinMarketCommission> GroupCoinMarketCommissions;///< Map of group-coin-market commission rules
	extern map<int, TradeRequestRobot> tradeRequestRobots;///< Map of trade request robot configurations
	extern map<int, LastTradePriceGenerator> lastTradePriceGenerators;///< Map of last trade price generators
	extern map<int, CommissionRateRule> commissionRateRules;///< Map of commission rate rules indexed by rule ID

	extern map<int,MarketObjects> marketObjects;///< Map of market-specific objects indexed by market ID
}