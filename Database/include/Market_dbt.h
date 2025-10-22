#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Market.h"


/**
 * @class Market_dbt
 * @brief Database table class for Market entity operations
 * 
 * This class provides database operations for the Market entity including
 * selection operations for market/exchange configurations. It extends
 * DatabaseTable to provide specific functionality for market-related
 * database operations.
 * 
 * Features:
 * - Market selection operations
 * - Individual and bulk market selection
 * - Exchange configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides market-specific database functionality
 * @note Supports both individual and bulk market operations
 */
class Market_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Market_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Market_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific market by ID
	 * 
	 * @param markets Reference to map to store the selected market
	 * @param Market_id ID of the market to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific market configuration from the database
	 * @note Populates the markets map with the selected market
	 * @note Returns false if market not found
	 */
	bool Select(map<int, Market>& markets,int Market_id);
	
	/**
	 * @brief Selects all markets and populates the provided map
	 * 
	 * @param markets Reference to map to store all markets indexed by market ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all market configurations from the database
	 * @note Populates the markets map with market ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, Market>& markets);
};