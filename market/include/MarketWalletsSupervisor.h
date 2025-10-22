/**
 * @file MarketWalletsSupervisor.h
 * @brief Supervisor for monitoring market wallet balances across exchanges
 *
 * This file contains the MarketWalletsSupervisor class which monitors and
 * manages wallet balances across different cryptocurrency exchanges.
 */

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
#include "include/types.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>
#include <include/errors.hpp>
#include "Coin.h"
#include "GlobalVariables.h"
#include "Configuration.h"
#include "MarketResponse.h"
#include "MarketResponseDetail.h"
#include "Enums.h"
#include "UtilityTools.h"
#include <cpr/cpr.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>
#include <Market.h>

/**
 * @class MarketWalletsSupervisor
 * @brief Supervisor for monitoring market wallet balances across exchanges
 *
 * This class extends AThread to provide continuous monitoring of wallet balances
 * across different cryptocurrency exchanges. It retrieves balance information
 * from various exchanges and updates the system accordingly.
 *
 * Features:
 * - Multi-exchange wallet balance monitoring
 * - Support for Binance, Nobitex, and Kucoin exchanges
 * - Thread-based continuous monitoring
 * - Balance update and publishing
 * - HTTP session management
 *
 * @note Inherits from AThread for thread management
 * @note Supports multiple cryptocurrency exchanges
 * @note Provides real-time balance monitoring
 */
class MarketWalletsSupervisor :public AThread
{
private:
    mutex mutex_;///< Mutex for thread safety
    cpr::Session session;///< HTTP session for API requests
    /**
     * @brief Main thread function for wallet balance monitoring
     *
     * @return unsigned Thread exit code
     *
     * @note Continuously monitors wallet balances
     * @note Handles balance updates and publishing
     * @note Manages exchange-specific balance retrieval
     */
    unsigned ThreadFunction();
    binapi::rest::api* api;///< Pointer to Binance REST API instance
    boost::asio::io_context ioctx;///< Boost.Asio I/O context for asynchronous operations
    Market market;///< Market configuration object
    /**
     * @brief Implements delay loop for thread execution
     *
     * @note Provides controlled timing for balance checks
     * @note Prevents excessive API calls
     */
    void Loop_delay();

    /**
     * @brief Retrieves wallet balance from Binance exchange
     *
     * @note Gets USDT and coin balances from Binance
     * @note Updates internal balance tracking
     * @note Handles API errors and exceptions
     */
    void Get_MarketWalletBalance_Binance();

    /**
     * @brief Retrieves wallet balance from Nobitex exchange
     *
     * @note Gets USDT and coin balances from Nobitex
     * @note Updates internal balance tracking
     * @note Handles API errors and exceptions
     */
    void Get_MarketWalletBalance_Nobitex();

    /**
     * @brief Retrieves wallet balance from Kucoin exchange
     *
     * @note Gets USDT and coin balances from Kucoin
     * @note Updates internal balance tracking
     * @note Handles API errors and exceptions
     */
    void Get_MarketWalletBalance_kucoin();

    /**
     * @brief Sets USDT balance in the system
     *
     * @param Usdt_balance USDT balance amount
     *
     * @note Updates USDT balance tracking
     * @note Thread-safe operation
     */
    void SetUsdtBalance(double Usdt_balance);

    /**
     * @brief Sets coin balance for a specific cryptocurrency
     *
     * @param coin Reference to Coin object
     * @param coin_balance Coin balance amount
     *
     * @note Updates coin balance tracking
     * @note Thread-safe operation
     */
    void SetCoinBalance(Coin& coin,double coin_balance);

    /**
     * @brief Publishes balance update messages
     *
     * @note Sends balance updates to system components
     * @note Triggers balance change notifications
     */
    void PublishMessages();

public:
    /**
     * @brief Constructor for MarketWalletsSupervisor
     *
     * @param _market Market configuration object
     *
     * @note Initializes market-specific monitoring
     * @note Sets up API connections
     * @note Configures balance tracking
     */
    MarketWalletsSupervisor(Market _market);

    /**
     * @brief Initiates wallet balance retrieval
     *
     * @note Triggers balance check for all supported exchanges
     * @note Updates system balance information
     * @note Publishes balance changes
     */
    void Get_MarketWalletBalance();
};
