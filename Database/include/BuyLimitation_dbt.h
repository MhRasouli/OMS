/**
 * @file BuyLimitation_dbt.h
 * @brief Database table class for BuyLimitation entity operations
 * 
 * This file contains the BuyLimitation_dbt class which provides
 * database operations for buy limitation configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "BuyLimitation.h"

/**
 * @class BuyLimitation_dbt
 * @brief Database table class for BuyLimitation entity operations
 * 
 * This class provides database operations for the BuyLimitation entity
 * including selection operations for buy limitation configurations.
 * It extends DatabaseTable to provide specific functionality for
 * buy limitation database operations.
 * 
 * Features:
 * - Buy limitation selection operations
 * - Individual and bulk buy limitation selection
 * - Buy limitation configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides buy limitation-specific database functionality
 * @note Supports both individual and bulk buy limitation operations
 */
class BuyLimitation_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for BuyLimitation_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	BuyLimitation_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific buy limitation by ID
	 * 
	 * @param buyLimitations Reference to map to store the selected buy limitation
	 * @param BuyLimitation_id ID of the buy limitation to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific buy limitation configuration from the database
	 * @note Populates the buyLimitations map with the selected buy limitation
	 * @note Returns false if buy limitation not found
	 */
	bool Select(map<int, BuyLimitation>& buyLimitations,int BuyLimitation_id);
	
	/**
	 * @brief Selects all buy limitations and populates the provided map
	 * 
	 * @param buyLimitations Reference to map to store all buy limitations indexed by limitation ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all buy limitation configurations from the database
	 * @note Populates the buyLimitations map with limitation ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, BuyLimitation>& buyLimitations);
};