/**
 * @file BinanceTrader.h
 * @brief Binance exchange trader implementation for executing market orders
 *
 * This file contains the BinanceTrader class which implements the MarketTrader
 * interface for executing trading operations on the Binance exchange.
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
#include "MarketTrader.h"
//###############################################################

/**
 * @def PRINT_IF_ERROR(res)
 * @brief Macro to print error messages from API responses
 * @param res API response object to check for errors
 */
#define PRINT_IF_ERROR(res) \
    if ( !static_cast<bool>(res) ) { \
        std::cout << __FILE__ << "(" << __LINE__ << "): msg=" << res.errmsg << std::endl; \
    }

//###############################################################

/**
 * @def BREAK_IF_ERROR(res)
 * @brief Macro to print and break on API response errors
 * @param res API response object to check for errors
 */
#define BREAK_IF_ERROR(res) \
    PRINT_IF_ERROR(res); 

//###############################################################

/**
 * @class BinanceTrader
 * @brief Binance exchange trader implementation for executing market orders
 *
 * This class implements the MarketTrader interface specifically for the Binance
 * cryptocurrency exchange. It provides functionality to execute market orders
 * using the Binance REST API.
 *
 * Features:
 * - Market order execution on Binance exchange
 * - REST API integration using binapi library
 * - Boost.Asio for asynchronous operations
 * - Error handling and response processing
 *
 * @note Inherits from MarketTrader for common trading interface
 * @note Uses binapi library for Binance API integration
 * @note Supports both buy and sell market orders
 */
class BinanceTrader : public MarketTrader
{
private:
    binapi::rest::api* api;///< Pointer to Binance REST API instance
    boost::asio::io_context ioctx;///< Boost.Asio I/O context for asynchronous operations

public:
    /**
     * @brief Constructor for BinanceTrader
     *
     * @param _coin Pointer to the Coin object representing the cryptocurrency to trade
     *
     * @note Initializes the Binance API connection
     * @note Sets up the I/O context for asynchronous operations
     */
    BinanceTrader(Coin* _coin);

    /**
     * @brief Executes a market order on the Binance exchange
     *
     * @param tradeRequest_id ID of the trade request
     * @param side Trading side (buy or sell)
     * @param quantity Amount to trade
     * @param commission_qty Commission quantity
     * @param price Price for the trade
     * @param coin_id ID of the cryptocurrency
     * @param qty_res Reference to store the actual executed quantity
     * @param price_res Reference to store the actual execution price
     * @param marketResponse Reference to store the market response
     * @param rejectReason Reference to store rejection reason if order fails
     * @return bool True if order was executed successfully, false otherwise
     *
     * @note Executes market orders using Binance REST API
     * @note Populates response parameters with execution details
     * @note Returns false if order is rejected or fails
     * @note Thread-safe operation
     */
    bool MarketOrder(int tradeRequest_id,side_enum side, double quantity,double commission_qty,double price,int coin_id,double& qty_res,double& price_res, MarketResponse& marketResponse,string& rejectReason) override;
};
