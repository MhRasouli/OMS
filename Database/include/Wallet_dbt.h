#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Wallet.h"
#include "Coin.h"
#include "AccountTpSl.h"

/**
 * @class Wallet_dbt
 * @brief Database table class for Wallet entity operations
 * 
 * This class provides comprehensive database operations for the Wallet entity
 * including selection, existence checking, and take profit/stop loss management.
 * It extends DatabaseTable to provide specific functionality for wallet-related
 * database operations.
 * 
 * Features:
 * - Wallet selection and existence checking
 * - Account and coin-specific wallet operations
 * - Take profit/stop loss (TP/SL) management
 * - Previous trades TP/SL loading
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides comprehensive wallet database functionality
 * @note Supports TP/SL and trade history management
 */
class Wallet_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Wallet_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Wallet_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Checks if a wallet exists for a specific account and coin
	 * 
	 * @param Result Reference to store the wallet if it exists
	 * @param Account_id ID of the account
	 * @param Coin_id ID of the coin
	 * @return bool True if wallet exists, false otherwise
	 * 
	 * @note Checks for wallet existence and populates Result if found
	 * @note Returns false if wallet doesn't exist
	 */
	bool Exist(Wallet& Result, int Account_id, int Coin_id);
	
	/**
	 * @brief Selects a specific wallet by ID
	 * 
	 * @param Wallet_id ID of the wallet to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves wallet information for the specified wallet ID
	 * @note Returns false if wallet not found
	 */
	bool Select(int Wallet_id);
	
	/**
	 * @brief Selects all wallets for a specific coin
	 * 
	 * @param coin Reference to the Coin object to populate with wallets
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all wallets for the specified coin
	 * @note Populates the coin's wallet map
	 * @note Returns false if selection fails
	 */
	bool SelectAll(Coin &coin);
	
	/**
	 * @brief Adds wallet to account take profit/stop loss management
	 * 
	 * @param wallet Wallet object to add to TP/SL management
	 * @return bool True if addition was successful, false otherwise
	 * 
	 * @note Adds wallet to account TP/SL tracking system
	 * @note Returns false if addition fails
	 */
	bool AddToAccountTpSls(const Wallet wallet);
	
	/**
	 * @brief Loads and adds previous trades TP/SL data for a wallet
	 * 
	 * @param wallet Wallet object to load TP/SL data for
	 * @param coinTpSl Reference to CoinTpSl object to populate
	 * @return bool True if loading was successful, false otherwise
	 * 
	 * @note Loads previous trades TP/SL data for the wallet
	 * @note Populates the coinTpSl object with historical data
	 * @note Returns false if loading fails
	 */
	bool LoadAdd_PreviousTrades_TpSls(const Wallet wallet,CoinTpSl& coinTpSl);
};