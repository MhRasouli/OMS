#pragma once

#include "stdafx.h"
#include "Coin.h"
#include "MarketResponse.h"

/**
 * @class MarketTrader
 * @brief Abstract base class for market trading operations
 * 
 * This class serves as the base class for all market trader implementations
 * that handle order execution on various cryptocurrency exchanges. It provides
 * a common interface for market order execution across different trading platforms.
 * 
 * Features:
 * - Abstract market order execution interface
 * - Coin reference for trading pair information
 * - Market response handling
 * - Commission quantity tracking
 * - Order rejection reason reporting
 * 
 * Derived classes implement specific exchange trading APIs (Binance, Kucoin, Nobitex, etc.)
 * and provide actual order execution functionality.
 * 
 * @note This is an abstract class - derived classes must implement MarketOrder()
 * @note Contains reference to Coin object for trading pair information
 * @note Supports both buy and sell side operations
 * @note Provides detailed market response and rejection information
 */
class MarketTrader
{
protected:
    Coin* coin;///< Pointer to the Coin object representing the trading pair

public:
    /**
     * @brief Constructor for MarketTrader
     * 
     * Initializes the market trader with a reference to the coin being traded.
     * 
     * @param _coin Pointer to the Coin object for the trading pair
     */
    MarketTrader(Coin* _coin);
    
    /**
     * @brief Pure virtual method for executing market orders
     * 
     * This method must be implemented by derived classes to execute market orders
     * on their respective exchanges. It handles the complete order lifecycle
     * including execution, commission calculation, and response processing.
     * 
     * @param tradeRequest_id Unique identifier for the trade request
     * @param side Buy or sell side of the order
     * @param quantity Amount of cryptocurrency to trade
     * @param commission_qty Commission amount to be deducted
     * @param price Price at which to execute the order
     * @param coin_id ID of the cryptocurrency being traded
     * @param qty_res Reference to store the actual executed quantity
     * @param price_res Reference to store the actual execution price
     * @param marketResponse Reference to store the market response details
     * @param rejectReason Reference to store rejection reason if order fails
     * @return bool True if order was successfully executed, false otherwise
     * 
     * @note Pure virtual - must be implemented by derived classes
     * @note Modifies qty_res, price_res, marketResponse, and rejectReason by reference
     * @note Handles both successful execution and failure scenarios
     */
    virtual bool MarketOrder(int tradeRequest_id,side_enum side, double quantity,double commission_qty,double price,int coin_id,double& qty_res,double& price_res, MarketResponse& marketResponse,string& rejectReason)=0;
};
