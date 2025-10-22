/**
 * @file KucoinTrader.h
 * @brief Kucoin exchange trader implementation using Python
 *
 * This file contains the KucoinTrader class which implements the MarketTrader
 * interface for executing trading operations on the Kucoin exchange using
 * Python integration.
 */

#pragma once

#include "stdafx.h"
#include "Logger.h"
#include "UtilityTools.h"
#include "MarketTrader.h"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

/**
 * @class KucoinTrader
 * @brief Kucoin exchange trader implementation using Python
 *
 * This class implements the MarketTrader interface specifically for the Kucoin
 * cryptocurrency exchange using Python integration through pybind11. It provides
 * functionality to execute market orders using Python libraries for Kucoin API access.
 *
 * Features:
 * - Market order execution on Kucoin exchange using Python
 * - pybind11 for C++/Python interoperability
 * - Order status tracking and detail retrieval
 * - Error handling and response processing
 *
 * @note Inherits from MarketTrader for common trading interface
 * @note Uses pybind11 for Python integration
 * @note Supports both buy and sell market orders
 * @note Leverages Python libraries for Kucoin API access
 */
class KucoinTrader : public MarketTrader
{
public:
    /**
     * @brief Constructor for KucoinTrader
     *
     * @param _coin Pointer to the Coin object representing the cryptocurrency to trade
     *
     * @note Initializes Python environment
     * @note Sets up Kucoin API access
     */
    KucoinTrader(Coin *_coin);

    /**
     * @brief Executes a market order on the Kucoin exchange
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
     * @note Executes market orders using Python Kucoin API
     * @note Populates response parameters with execution details
     * @note Returns false if order is rejected or fails
     * @note Thread-safe operation
     */
    bool MarketOrder(int tradeRequest_id, side_enum side, double quantity, double commission_qty, double price, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason) override;
    
private:
    /**
     * @brief Sends a market order to Kucoin exchange
     *
     * @param tradeRequest_id ID of the trade request
     * @param side Trading side (buy or sell)
     * @param quantity Amount to trade
     * @return string Order ID returned by Kucoin exchange
     *
     * @note Sends order using Python Kucoin API
     * @note Returns order ID for tracking
     * @note Handles API errors and exceptions
     */
    string Send_market_order(int tradeRequest_id,side_enum side, double quantity);

    /**
     * @brief Retrieves order details from Kucoin exchange
     *
     * @param orderId Order ID to retrieve details for
     * @param quantity Expected quantity for validation
     * @param qty_res Reference to store the actual executed quantity
     * @param price_res Reference to store the actual execution price
     * @param marketResponse Reference to store the market response
     * @return bool True if order details retrieved successfully, false otherwise
     *
     * @note Retrieves order status and execution details
     * @note Populates response parameters with order information
     * @note Validates order execution
     * @note Returns false if order not found or failed
     */
    bool Get_order_detail(string orderId,double quantity, double &qty_res, double &price_res, MarketResponse &marketResponse);
};
