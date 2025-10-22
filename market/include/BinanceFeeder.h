#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "FeedCheckerThread.h"
#include "Logger.h"
#include "Configuration.h"

#include "include/websocket.hpp"
#include "include/api.hpp"
#include "include/pairslist.hpp"
#include "include/reports.hpp"
#include "include/flatjson.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>
#include <include/errors.hpp>
#include "Coin.h"
#include "GlobalVariables.h"
#include "PriceFeeder.h"


/**
 * @class BinanceFeeder
 * @brief Binance exchange price data feeder implementation
 * 
 * This class implements the PriceFeeder interface specifically for the Binance
 * cryptocurrency exchange. It provides real-time price data through WebSocket
 * connections and manages the continuous streaming of market data.
 * 
 * Features:
 * - Real-time WebSocket connection to Binance API
 * - Book ticker data streaming for bid/ask prices
 * - Feed checker thread for data validation
 * - Boost.Asio I/O context for asynchronous operations
 * - Automatic data freshness tracking
 * - Thread-safe price data updates
 * 
 * The feeder connects to Binance's WebSocket API to receive real-time
 * price updates for the specified trading pair and maintains the connection
 * until explicitly stopped.
 * 
 * @note Inherits from PriceFeeder and implements AThread functionality
 * @note Uses Binance API WebSocket for real-time data streaming
 * @note Includes feed checker for data validation and monitoring
 * @note Manages Boost.Asio I/O context for asynchronous operations
 */
class BinanceFeeder : public PriceFeeder
{
public:
	/**
	 * @brief Destructor for BinanceFeeder
	 * 
	 * Cleans up resources and ensures proper shutdown of WebSocket connections
	 * and associated threads.
	 */
	~BinanceFeeder();
	
	/**
	 * @brief Constructor for BinanceFeeder
	 * 
	 * Initializes the Binance feeder with a reference to the coin being monitored.
	 * Sets up the WebSocket connection and feed checker thread.
	 * 
	 * @param Coin_ptr Pointer to the Coin object for the trading pair
	 */
	BinanceFeeder(Coin* Coin_ptr);
	
	/**
	 * @brief Stops data retrieval from Binance WebSocket
	 * 
	 * Implements the pure virtual method from PriceFeeder to stop
	 * the WebSocket connection and clean up resources.
	 * 
	 * @note Overrides PriceFeeder::StopGetData()
	 * @note Stops WebSocket connection and associated threads
	 */
	void StopGetData() override;
	
	/**
	 * @brief Initializes the book ticker WebSocket stream
	 * 
	 * Sets up the WebSocket connection to Binance's book ticker stream
	 * for real-time bid/ask price updates.
	 * 
	 * @note Connects to Binance WebSocket API
	 * @note Subscribes to book ticker stream for the trading pair
	 */
	void InitBookTicker();
	
	Coin* coin_ptr;///< Pointer to the Coin object for the trading pair

private:
	
	binapi::ws::websockets wsp;///< Binance WebSocket client for real-time data streaming
	FeedCheckerThread fedchkThread;///< Thread for monitoring and validating feed data
	unsigned ThreadFunction();///< Main thread function for price data processing
	bool dataIsNew;///< Flag indicating if new price data has been received
	boost::asio::io_context ioctx;///< Boost.Asio I/O context for asynchronous operations

};