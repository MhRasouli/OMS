#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct WalletHistory
 * @brief Data structure representing wallet balance history and changes
 * 
 * This structure contains historical information about wallet balance changes
 * including before/after balances for both USD and cryptocurrency quantities.
 * It tracks the history of wallet modifications through trades and deposits/withdrawals.
 * 
 * Key Features:
 * - Wallet history identification and tracking
 * - Account and trade associations
 * - Before/after balance tracking (USD and coin quantities)
 * - Timestamp for historical record
 * - Deposit/withdrawal association
 * 
 * @note Contains comprehensive wallet balance history
 * @note Tracks both USD and cryptocurrency balance changes
 * @note Links to trades and deposit/withdrawal operations
 */
struct WalletHistory
{
	int walletHistory_id = 0;///< Unique identifier for the wallet history record
	int account_id = 0;///< ID of the account whose wallet changed
	int trade_id = 0;///< ID of the trade that caused the balance change
	double balanceBefore_usd = 0;///< USD balance before the change
	double balanceAfter_usd = 0;///< USD balance after the change
	int coin_id = 0;///< ID of the cryptocurrency whose quantity changed
	double coinBefore_qty = 0;///< Coin quantity before the change
	double coinAfter_qty = 0;///< Coin quantity after the change
	time_t time = 0;///< Timestamp when the balance change occurred
	int depositWithdrawal_id=0;///< ID of the deposit/withdrawal that caused the change (if applicable)
};