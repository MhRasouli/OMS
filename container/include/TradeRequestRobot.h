#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct TradeRequestRobot
 * @brief Data structure representing automated trading robot configuration
 * 
 * This structure contains comprehensive configuration parameters for
 * automated trading robots. It defines pricing strategies, timing controls,
 * queue management, and interaction rules for automated trading operations.
 * 
 * Key Features:
 * - Trading robot identification and account association
 * - Price and quantity precision settings
 * - Distance-based pricing strategies
 * - Action and delay timing controls
 * - Queue length and order management
 * - Best request interaction rules
 * - Order keeping and trading probability settings
 * 
 * @note Contains comprehensive automated trading configuration
 * @note Defines pricing, timing, and interaction strategies
 * @note Supports sophisticated trading robot behavior
 */
struct TradeRequestRobot
{
    int tradeRequestRobot_id = 0;///< Unique identifier for the trading robot
    int account_id = 0;///< ID of the account associated with this robot
    int price_precision = 0;///< Decimal precision for price calculations
    int quantity_precision = 0;///< Decimal precision for quantity calculations
    double price_distanceFrombestRequest_min_perc = 0;///< Minimum price distance from best request as percentage
    double price_distanceFrombestRequest_max_perc = 0;///< Maximum price distance from best request as percentage
    double quantity_distanceFrombestRequest_min_usdt = 0;///< Minimum quantity distance from best request in USDT
    double quantity_distanceFrombestRequest_max_usdt = 0;///< Maximum quantity distance from best request in USDT
    int actionDelay_min_sec = 0;///< Minimum action delay in seconds
    int actionDelay_max_sec = 0;///< Maximum action delay in seconds
    int noOrderDelay_min_sec = 0;///< Minimum delay when no orders are present in seconds
    int noOrderDelay_max_sec = 0;///< Maximum delay when no orders are present in seconds
    int maxQueueLength = 0;///< Maximum queue length for the robot
    double minPriceDifferenceOfBestBuySell_usdt = 0;///< Minimum price difference between best buy and sell in USDT
    bool enable = false;///< Flag indicating if the robot is enabled
    int KeepOrder_maxDistanceFromBestRequest_num = 0;///< Maximum distance from best request to keep order
    double KeepOrder_maxPriceDifferenceFromBestRequest_perc = 0;///< Maximum price difference from best request to keep order as percentage
    int KeepOrder_maxTime_sec=0;///< Maximum time to keep an order in seconds
    double tradeWithBestReq_minPriceDiffSecondReqFromOppositeBestReq_perc = 0;///< Minimum price difference for trading with best request as percentage
    double probabilityOfTradeWithOppositeRobot_perc=0;///< Probability of trading with opposite robot as percentage
};
