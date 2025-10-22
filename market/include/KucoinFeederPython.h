/**
 * @file KucoinFeederPython.h
 * @brief Kucoin exchange price feeder implementation using Python
 *
 * This file contains the KucoinFeederPython class which implements the PriceFeeder
 * interface for retrieving real-time price data from the Kucoin exchange using
 * Python integration.
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
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

/**
 * @class KucoinFeederPython
 * @brief Kucoin exchange price feeder implementation using Python
 *
 * This class implements the PriceFeeder interface specifically for the Kucoin
 * cryptocurrency exchange using Python integration through pybind11. It provides
 * real-time price data by leveraging Python libraries for Kucoin API access.
 *
 * Features:
 * - Real-time price data from Kucoin using Python integration
 * - pybind11 for C++/Python interoperability
 * - Price feed monitoring and error handling
 * - Thread-based data retrieval
 *
 * @note Inherits from PriceFeeder for common price feeding interface
 * @note Uses pybind11 for Python integration
 * @note Provides thread-safe price data updates
 * @note Leverages Python libraries for Kucoin API access
 */
class KucoinFeederPython : public PriceFeeder
{
public:
    /**
     * @brief Destructor for KucoinFeederPython
     *
     * @note Cleans up Python resources
     * @note Stops monitoring threads
     */
    ~KucoinFeederPython();

    /**
     * @brief Constructor for KucoinFeederPython
     *
     * @param Coin_ptr Pointer to the Coin object to feed price data for
     *
     * @note Initializes Python environment
     * @note Sets up price data monitoring
     * @note Starts price data retrieval
     */
    KucoinFeederPython(Coin *Coin_ptr);

    Coin *coin_ptr;///< Pointer to the Coin object being fed

    /**
     * @brief Stops retrieving price data
     *
     * @note Stops monitoring threads
     * @note Cleans up Python resources
     * @note Terminates price data retrieval
     */
    void StopGetData() override;

private:
    FeedCheckerThread fedchkThread;///< Thread for monitoring feed data freshness
    /**
     * @brief Main thread function for price data retrieval
     *
     * @return unsigned Thread exit code
     *
     * @note Manages Python-based price data retrieval
     * @note Handles price data updates
     * @note Manages Python API calls
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
};