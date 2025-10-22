/**
 * @file KucoinTokenManager.h
 * @brief Kucoin WebSocket token management for authentication
 *
 * This file contains the KucoinTokenManager class which manages authentication
 * tokens for Kucoin WebSocket connections, including automatic token renewal.
 */

#pragma once

#include <string>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>

/**
 * @class KucoinTokenManager
 * @brief Kucoin WebSocket token management for authentication
 *
 * This class manages authentication tokens for Kucoin WebSocket connections.
 * It handles token acquisition, renewal, and provides WebSocket URLs with
 * proper authentication parameters.
 *
 * Features:
 * - Token acquisition and management
 * - Automatic token renewal
 * - Thread-safe token operations
 * - WebSocket URL generation with authentication
 * - Token expiry tracking
 *
 * @note Thread-safe implementation with mutex protection
 * @note Automatic background token renewal
 * @note Provides authenticated WebSocket URLs
 */
class KucoinTokenManager
{
public:
    /**
     * @brief Constructor for KucoinTokenManager
     *
     * @param api_url Kucoin API URL for token requests
     *
     * @note Initializes token management
     * @note Sets up API endpoint
     * @note Starts automatic token renewal
     */
    KucoinTokenManager(const std::string &api_url);

    /**
     * @brief Destructor for KucoinTokenManager
     *
     * @note Stops automatic token renewal
     * @note Cleans up resources
     */
    ~KucoinTokenManager();

    /**
     * @brief Gets WebSocket URL with authentication token
     *
     * @return std::string WebSocket URL with authentication parameters
     *
     * @note Returns authenticated WebSocket URL
     * @note Includes current valid token
     * @note Thread-safe operation
     */
    std::string get_ws_url();

    /**
     * @brief Starts automatic token renewal
     *
     * @note Begins background token renewal thread
     * @note Ensures continuous authentication
     */
    void start_auto_renew();

    /**
     * @brief Stops automatic token renewal
     *
     * @note Terminates background renewal thread
     * @note Stops automatic token refresh
     */
    void stop_auto_renew();

    /**
     * @brief Manually renews authentication token
     *
     * @note Requests new token from Kucoin API
     * @note Updates token and expiry time
     * @note Thread-safe operation
     */
    void renew_token();

private:
    std::mutex mutex_;///< Mutex for thread safety
    std::string m_token;///< Current authentication token
    std::string m_endpoint;///< WebSocket endpoint URL
    std::mutex m_mutex;///< Additional mutex for synchronization
    std::chrono::system_clock::time_point m_expiry;///< Token expiry time
    std::string m_api_url;///< Kucoin API URL for token requests

    std::atomic<bool> m_running{true};///< Flag indicating if renewal is active
    std::thread m_renew_thread;///< Background thread for token renewal
};