#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "CommissionRate.h"

/**
 * @class CommissionRate_dbt
 * @brief Database table class for CommissionRate entity operations
 * 
 * This class provides database operations for the CommissionRate entity
 * including selection operations for commission rate configurations.
 * It extends DatabaseTable to provide specific functionality for
 * commission rate database operations.
 * 
 * Features:
 * - Commission rate selection operations
 * - Individual and bulk commission rate selection
 * - Commission rate configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides commission rate-specific database functionality
 * @note Supports both individual and bulk commission rate operations
 */
class CommissionRate_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for CommissionRate_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	CommissionRate_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific commission rate by ID
	 * 
	 * @param commissionRates Reference to map to store the selected commission rate
	 * @param commissionRate_id ID of the commission rate to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific commission rate configuration from the database
	 * @note Populates the commissionRates map with the selected commission rate
	 * @note Returns false if commission rate not found
	 */
	bool Select(map<int, CommissionRate> &commissionRates,int commissionRate_id);
	
	/**
	 * @brief Selects all commission rates and populates the provided map
	 * 
	 * @param commissionRates Reference to map to store all commission rates indexed by rate ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all commission rate configurations from the database
	 * @note Populates the commissionRates map with commission rate ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, CommissionRate> &commissionRates);
};