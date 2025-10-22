/**
 * @file Change.h
 * @brief Data structure for tracking database changes and modifications
 * 
 * This file contains the Change structure used for tracking modifications
 * to various database entities throughout the trading system.
 */

#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct Change
 * @brief Data structure for tracking database changes and entity modifications
 * 
 * This structure is used to track changes made to various entities in the
 * trading system. It contains references to different types of entities
 * that may have been modified, allowing the system to process and synchronize
 * changes across different components.
 * 
 * Key Features:
 * - Change identification and tracking
 * - References to various entity types
 * - Deletion status tracking
 * - Support for multiple entity types in a single change record
 * 
 * @note Used for change tracking and synchronization
 * @note Contains references to multiple entity types
 * @note Supports deletion status tracking
 */
struct Change
{
	int change_id = 0;///< Unique identifier for the change record
	bool deleted = false;///< Flag indicating if the change represents a deletion
	int tradeRequest_id = 0;///< ID of the trade request that was changed
	int wallet_id = 0;///< ID of the wallet that was changed
	int account_id = 0;///< ID of the account that was changed
	int coin_id = 0;///< ID of the coin that was changed
	int tradeRequestGenerator_id = 0;///< ID of the trade request generator that was changed
	int market_id = 0;///< ID of the market that was changed
	int commissionRate_id = 0;///< ID of the commission rate that was changed
	int stairCommission_id = 0;///< ID of the stair commission that was changed
	int depositWithdrawal_id = 0;///< ID of the deposit/withdrawal that was changed
	int coinMinBalance_alertMessageThreshold_id = 0;///< ID of the coin minimum balance threshold that was changed
	int group_id = 0;///< ID of the group that was changed
	int buyLimitation_id = 0;///< ID of the buy limitation that was changed
	int groupCoinMarketCommission_id = 0;///< ID of the group coin market commission that was changed
	int tradeRequestRobot_id = 0;///< ID of the trade request robot that was changed
	int lastTradePriceGenerator_id=0;///< ID of the last trade price generator that was changed
	int commissionRateRule_id=0;///< ID of the commission rate rule that was changed
};