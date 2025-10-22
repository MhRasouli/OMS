#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct Account
 * @brief Data structure representing a trading account in the system
 * 
 * This structure contains account-specific information including balance,
 * trading history, and group associations. It serves as the data model
 * for user accounts in the trading service.
 * 
 * Key Features:
 * - Account identification and metadata
 * - USDT balance tracking
 * - Monthly trading volume tracking
 * - Group membership for account organization
 * 
 * @note Simple data structure for account information
 * @note Tracks USDT balance and trading volume
 * @note Supports group-based account organization
 */
struct Account
{
	int account_id = 0;///< Unique identifier for the account
	double balance_usdt = 0.0;///< Current USDT balance in the account
	double tradePrice_lastMonth_usdt=0.0;///< Total trading volume in USDT for the last month
	int group_id=0;///< ID of the group this account belongs to
};