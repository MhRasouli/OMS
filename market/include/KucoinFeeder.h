/**
 * @file KucoinFeeder.h
 * @brief Kucoin exchange price feeder implementation
 *
 * This file contains the KucoinFeeder class which implements the PriceFeeder
 * interface for retrieving real-time price data from the Kucoin exchange.
 */

#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "FeedCheckerThread.h"
#include "Logger.h"
#include "Configuration.h"
#include "Coin.h"
#include "GlobalVariables.h"
#include "PriceFeeder.h"
#include "KucoinTokenManager.h"
#include "KucoinWebSocketFeed.h"
#include <memory>
#include <map>
#include <thread>

/**
 * @class KucoinFeeder
 * @brief Kucoin exchange price feeder implementation
 *
 * This class implements the PriceFeeder interface specifically for the Kucoin
 * cryptocurrency exchange. It provides real-time price data through WebSocket
 * connections and manages authentication tokens.
 *
 * Features:
 * - Real-time price data from Kucoin WebSocket API
 * - Token-based authentication management
 * - WebSocket connection management
 * - Price feed monitoring and error handling
 * - Automatic token renewal
 *
 * @note Inherits from PriceFeeder for common price feeding interface
 * @note Uses WebSocket connections for real-time data
 * @note Manages authentication tokens automatically
 * @note Provides thread-safe price data updates
 */
class KucoinFeeder : public PriceFeeder
{
public:
    /**
     * @brief Destructor for KucoinFeeder
     *
     * @note Cleans up WebSocket connections and resources
     * @note Stops monitoring threads
     */
    ~KucoinFeeder();

    /**
     * @brief Constructor for KucoinFeeder
     *
     * @param Coin_ptr Pointer to the Coin object to feed price data for
     *
     * @note Initializes WebSocket connections
     * @note Sets up token management
     * @note Starts price data monitoring
     */
    KucoinFeeder(Coin *Coin_ptr);

    Coin *coin_ptr;///< Pointer to the Coin object being fed

    /**
     * @brief Stops retrieving price data
     *
     * @note Closes WebSocket connections
     * @note Stops monitoring threads
     * @note Cleans up resources
     */
    void StopGetData() override;

    /**
     * @brief Renews authentication token
     *
     * @note Refreshes expired authentication tokens
     * @note Maintains continuous WebSocket connection
     * @note Handles token renewal errors
     */
    void RenewToken();

private:
    FeedCheckerThread fedchkThread;///< Thread for monitoring feed data freshness
    /**
     * @brief Main thread function for price data retrieval
     *
     * @return unsigned Thread exit code
     *
     * @note Manages WebSocket connections
     * @note Handles price data updates
     * @note Manages token renewal
     */
    unsigned ThreadFunction();
    bool dataIsNew;///< Flag indicating if price data is fresh
    /**
     * @brief Implements delay loop for thread execution
     *
     * @note Provides controlled timing for data retrieval
     * @note Prevents excessive API calls
     */
    void Loop_delay();
    
    // WebSocket components
    std::unique_ptr<KucoinTokenManager> m_token_manager;///< Unique pointer to token manager
    std::unique_ptr<KucoinWebSocketFeed> m_websocket_feed;///< Unique pointer to WebSocket feed
    std::atomic<bool> m_websocket_initialized{false};///< Atomic flag for WebSocket initialization status
    
    /**
     * @brief Callback for price update events
     *
     * @param symbol Trading symbol
     * @param bid_price Current bid price
     * @param bid_quantity Bid quantity
     * @param ask_price Current ask price
     * @param ask_quantity Ask quantity
     *
     * @note Updates coin price data
     * @note Thread-safe price updates
     * @note Handles price data validation
     */
    void on_price_update(const std::string& symbol, double bid_price, double bid_quantity, 
                        double ask_price, double ask_quantity);
    
    /**
     * @brief Initializes WebSocket connection
     *
     * @note Sets up WebSocket feed
     * @note Configures price update callbacks
     * @note Handles initialization errors
     */
    void initialize_websocket();

    /**
     * @brief Cleans up WebSocket connection
     *
     * @note Closes WebSocket connections
     * @note Releases WebSocket resources
     * @note Resets initialization flags
     */
    void cleanup_websocket();
};