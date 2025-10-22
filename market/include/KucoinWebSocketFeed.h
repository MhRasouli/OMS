/**
 * @file KucoinWebSocketFeed.h
 * @brief WebSocket feed implementation for Kucoin exchange price data
 *
 * This file contains the KucoinWebSocketFeed class which provides real-time
 * price data from the Kucoin exchange using WebSocket connections.
 */

#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>

typedef websocketpp::client<websocketpp::config::asio_tls_client> ws_client;///< WebSocket client type definition
using websocketpp::connection_hdl;///< Connection handle type definition
using json = nlohmann::json;///< JSON type definition

/**
 * @class KucoinWebSocketFeed
 * @brief WebSocket feed implementation for Kucoin exchange price data
 *
 * This class provides real-time price data from the Kucoin exchange using
 * WebSocket connections. It handles connection management, message processing,
 * and price data callbacks.
 *
 * Features:
 * - Real-time price data via WebSocket
 * - Multiple symbol subscription support
 * - Automatic connection management
 * - Price update callbacks
 * - Ping/pong heartbeat mechanism
 * - Thread-safe operations
 *
 * @note Uses websocketpp library for WebSocket connections
 * @note Supports TLS-secured connections
 * @note Provides callback-based price updates
 */
class KucoinWebSocketFeed
{
public:
    /**
     * @brief Constructor for KucoinWebSocketFeed
     *
     * @param url WebSocket URL for Kucoin feed
     * @param symbols Vector of trading symbols to subscribe to
     *
     * @note Initializes WebSocket client
     * @note Sets up symbol subscriptions
     * @note Configures connection parameters
     */
    KucoinWebSocketFeed(const std::string &url, const std::vector<std::string> &symbols);

    /**
     * @brief Destructor for KucoinWebSocketFeed
     *
     * @note Stops WebSocket connections
     * @note Cleans up threads and resources
     */
    ~KucoinWebSocketFeed();

    /**
     * @brief Runs the WebSocket feed synchronously
     *
     * @note Blocks until connection is established
     * @note Processes messages in current thread
     */
    void run();

    /**
     * @brief Runs the WebSocket feed asynchronously in a separate thread
     *
     * @note Starts background thread for WebSocket operations
     * @note Non-blocking operation
     */
    void run_async();

    /**
     * @brief Stops the WebSocket feed
     *
     * @note Closes WebSocket connections
     * @note Stops background threads
     * @note Terminates message processing
     */
    void stop();

    /**
     * @brief Waits for the WebSocket feed to complete
     *
     * @note Blocks until all threads are finished
     * @note Ensures clean shutdown
     */
    void wait();

    /**
     * @typedef PriceUpdateCallback
     * @brief Callback function type for price updates
     *
     * @param symbol Trading symbol
     * @param bid_price Current bid price
     * @param bid_quantity Bid quantity
     * @param ask_price Current ask price
     * @param ask_quantity Ask quantity
     */
    using PriceUpdateCallback = std::function<void(const std::string &symbol, double bid_price, double bid_quantity,
                                                   double ask_price, double ask_quantity)>;

    /**
     * @brief Sets the price update callback function
     *
     * @param callback Function to call when price data is received
     *
     * @note Registers callback for price updates
     * @note Called for each price data message
     */
    void set_price_callback(PriceUpdateCallback callback);

private:
    std::thread m_thread;///< Background thread for WebSocket operations
    /**
     * @brief Extracts base symbol from input string
     *
     * @param input Input symbol string
     * @return std::string Extracted base symbol
     *
     * @note Processes symbol formatting
     * @note Handles symbol normalization
     */
    std::string extractBaseSymbol(const std::string &input);

    /**
     * @brief Subscribes to price data for symbols
     *
     * @param hdl WebSocket connection handle
     *
     * @note Sends subscription messages
     * @note Configures symbol subscriptions
     */
    void subscribe(connection_hdl hdl);

    /**
     * @brief Handles incoming WebSocket messages
     *
     * @param hdl WebSocket connection handle
     * @param payload Message payload
     *
     * @note Processes price data messages
     * @note Triggers price update callbacks
     */
    void handle_message(connection_hdl hdl, const std::string &payload);

    /**
     * @brief Starts ping loop for connection maintenance
     *
     * @param hdl WebSocket connection handle
     *
     * @note Maintains connection heartbeat
     * @note Prevents connection timeout
     */
    void start_ping_loop(connection_hdl hdl);

    /**
     * @brief Processes ticker data from WebSocket messages
     *
     * @param symbol Trading symbol
     * @param data JSON data containing price information
     *
     * @note Extracts price data from JSON
     * @note Triggers price update callbacks
     */
    void process_ticker_data(std::string symbol, const json &data);

    ws_client m_client;///< WebSocket client instance
    std::string m_url;///< WebSocket URL
    std::vector<std::string> m_symbols;///< List of subscribed symbols

    std::atomic<bool> m_running{true};///< Flag indicating if feed is running
    std::thread m_ping_thread;///< Thread for ping operations
    std::atomic<bool> m_connected{false};///< Flag indicating connection status

    PriceUpdateCallback m_price_callback;///< Price update callback function
    connection_hdl m_current_connection;///< Current WebSocket connection handle
};