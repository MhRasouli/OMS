#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "CoinMinBalance_alertMessageThreshold.h"


/**
 * @class CoinMinBalance_alertMessageThreshold_dbt
 * @brief Database table class for CoinMinBalance_alertMessageThreshold entity operations
 * 
 * This class provides database operations for the CoinMinBalance_alertMessageThreshold
 * entity including selection operations for alert threshold configurations.
 * It extends DatabaseTable to provide specific functionality for alert threshold
 * database operations.
 * 
 * Features:
 * - Alert threshold selection operations
 * - Individual and bulk alert threshold selection
 * - Alert threshold configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides alert threshold-specific database functionality
 * @note Supports both individual and bulk alert threshold operations
 */
class CoinMinBalance_alertMessageThreshold_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for CoinMinBalance_alertMessageThreshold_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	CoinMinBalance_alertMessageThreshold_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific alert threshold by ID
	 * 
	 * @param coinMinBalances Reference to map to store the selected alert threshold
	 * @param coinMinBalance_id ID of the alert threshold to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific alert threshold configuration from the database
	 * @note Populates the coinMinBalances map with the selected alert threshold
	 * @note Returns false if alert threshold not found
	 */
	bool Select(map<int, CoinMinBalance_alertMessageThreshold>& coinMinBalances,int coinMinBalance_id);
	
	/**
	 * @brief Selects all alert thresholds and populates the provided map
	 * 
	 * @param coinMinBalances Reference to map to store all alert thresholds indexed by threshold ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all alert threshold configurations from the database
	 * @note Populates the coinMinBalances map with threshold ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, CoinMinBalance_alertMessageThreshold>& coinMinBalances);
};