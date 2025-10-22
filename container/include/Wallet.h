#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct Wallet
 * @brief Data structure representing a cryptocurrency wallet for a specific account
 * 
 * This structure contains wallet information linking an account to a specific
 * cryptocurrency with its current quantity. It serves as the data model for
 * cryptocurrency holdings within user accounts.
 * 
 * Key Features:
 * - Wallet identification and metadata
 * - Account and coin associations
 * - Quantity tracking for cryptocurrency holdings
 * 
 * @note Simple data structure for wallet information
 * @note Links accounts to specific cryptocurrencies
 * @note Tracks current quantity of cryptocurrency holdings
 */
struct Wallet
{
	int wallet_id = 0;///< Unique identifier for the wallet
	int account_id = 0;///< ID of the account that owns this wallet
	int coin_id = 0;///< ID of the cryptocurrency stored in this wallet
	double quantity = 0;///< Current quantity of the cryptocurrency in this wallet
};