#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Coin.h"
#include "BinanceFeeder.h"

/**
 * @class Coin_dbt
 * @brief Database table class for Coin entity operations
 * 
 * This class provides database operations for the Coin entity including
 * selection operations for individual coins and all coins. It extends
 * DatabaseTable to provide specific functionality for coin-related database operations.
 * 
 * Features:
 * - Coin selection operations
 * - Individual coin retrieval
 * - Bulk coin selection
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides coin-specific database functionality
 * @note Supports both individual and bulk coin operations
 */
class Coin_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Coin_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Coin_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific coin by ID and updates the coins map
	 * 
	 * @param coins Reference to map of coins to update
	 * @param Coin_id ID of the coin to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves coin information for the specified coin ID
	 * @note Updates the coins map with the selected coin
	 * @note Returns false if coin not found
	 */
	bool Select(map<int, Coin>& coins,int Coin_id);
	
	/**
	 * @brief Selects all coins and populates the provided map
	 * 
	 * @param coins Reference to map to store all coins indexed by coin ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all coins from the database
	 * @note Populates the coins map with coin ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, Coin>& coins);
};