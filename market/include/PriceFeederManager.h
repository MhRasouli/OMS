#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "BinanceFeeder.h"
#include "NobitexFeeder.h"
#include "KucoinFeeder.h"
#include "KucoinFeederPython.h"

/**
 * @class PriceFeederManager
 * @brief Manager class for coordinating price data feeders across multiple exchanges
 * 
 * This class manages and coordinates multiple price feeders for different cryptocurrency
 * exchanges and trading pairs. It provides centralized control over price data collection
 * and ensures proper initialization and lifecycle management of all price feeders.
 * 
 * Features:
 * - Centralized management of multiple price feeders
 * - Exchange-specific feeder initialization (Binance, Kucoin, Nobitex, etc.)
 * - Threaded execution for continuous price monitoring
 * - WebSocket box initialization for trading pairs
 * - Lifecycle management of price feeder instances
 * 
 * The manager creates and maintains price feeders for each enabled trading pair
 * across different exchanges, ensuring real-time price data is available for
 * trading decisions and order execution.
 * 
 * @note Inherits from AThread for threaded execution
 * @note Manages multiple exchange-specific price feeders
 * @note Initializes WebSocket connections for each trading pair
 * @note Coordinates price data collection across the entire system
 */
class PriceFeederManager : public AThread
{
public:
	/**
	 * @brief Default constructor for PriceFeederManager
	 * 
	 * Initializes the price feeder manager with default values and prepares
	 * for managing multiple price feeders across different exchanges.
	 */
	PriceFeederManager();
	
	/**
	 * @brief Initializes WebSocket box for a specific coin/trading pair
	 * 
	 * Creates and configures price feeders for the specified coin across
	 * all enabled exchanges. Sets up WebSocket connections and starts
	 * real-time price data streaming.
	 * 
	 * @param coin Pointer to the Coin object representing the trading pair
	 * 
	 * @note Creates exchange-specific feeders (Binance, Kucoin, Nobitex, etc.)
	 * @note Initializes WebSocket connections for real-time data streaming
	 * @note Starts price monitoring threads for the trading pair
	 */
	void InitCoinWebsocketBox(Coin* coin);

private:
	/**
	 * @brief Main thread function for price feeder management
	 * 
	 * Implements the pure virtual method from AThread. Manages the lifecycle
	 * of price feeders, monitors their status, and coordinates price data
	 * collection across all trading pairs and exchanges.
	 * 
	 * @return unsigned Return value from thread execution (currently unused)
	 * 
	 * @note Overrides AThread::ThreadFunction()
	 * @note Manages continuous price data collection
	 * @note Handles feeder lifecycle and error recovery
	 */
	unsigned ThreadFunction();
};
