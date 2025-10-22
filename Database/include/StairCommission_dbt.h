
#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "StairCommission.h"

/**
 * @class StairCommission_dbt
 * @brief Database table class for StairCommission entity operations
 * 
 * This class provides database operations for the StairCommission entity
 * including selection operations for tiered commission configurations.
 * It extends DatabaseTable to provide specific functionality for
 * stair commission database operations.
 * 
 * Features:
 * - Stair commission selection operations
 * - Individual and bulk stair commission selection
 * - Tiered commission configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides stair commission-specific database functionality
 * @note Supports both individual and bulk stair commission operations
 */
class StairCommission_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for StairCommission_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	StairCommission_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific stair commission by ID
	 * 
	 * @param stairCommissions Reference to list to store the selected stair commission
	 * @param stairCommission_id ID of the stair commission to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific stair commission configuration from the database
	 * @note Populates the stairCommissions list with the selected stair commission
	 * @note Returns false if stair commission not found
	 */
	bool Select(list<StairCommission>& stairCommissions,int stairCommission_id);
	
	/**
	 * @brief Selects all stair commissions and populates the provided list
	 * 
	 * @param stairCommissions Reference to list to store all stair commissions
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all stair commission configurations from the database
	 * @note Populates the stairCommissions list with all stair commissions
	 * @note Returns false if selection fails
	 */
	bool SelectAll(list<StairCommission>& stairCommissions);
};