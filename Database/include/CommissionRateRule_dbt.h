/**
 * @file CommissionRateRule_dbt.h
 * @brief Database table class for CommissionRateRule entity operations
 *
 * This file contains the CommissionRateRule_dbt class which provides
 * database operations for commission rate rule configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "CommissionRateRule.h"

/**
 * @class CommissionRateRule_dbt
 * @brief Database table class for CommissionRateRule entity operations
 *
 * This class provides database operations for the CommissionRateRule entity
 * including selection operations for commission rate rule configurations.
 * It extends DatabaseTable to provide specific functionality for commission
 * rate rule database operations.
 *
 * Features:
 * - Commission rate rule selection operations
 * - Individual and bulk rule selection
 * - Rule configuration management
 *
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides commission rate rule-specific database functionality
 * @note Supports both individual and bulk rule operations
 */
class CommissionRateRule_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for CommissionRateRule_dbt
	 *
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	CommissionRateRule_dbt(DatabaseHandler* databaseHandler);

	/**
	 * @brief Selects a specific commission rate rule by ID
	 *
	 * @param commissionRateRules Reference to map to store the selected rule
	 * @param commissionRateRule_id ID of the commission rate rule to select
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves specific commission rate rule configuration from the database
	 * @note Populates the commissionRateRules map with the selected rule
	 * @note Returns false if rule not found
	 */
	bool Select(map<int, CommissionRateRule> &commissionRateRules,int commissionRateRule_id);

	/**
	 * @brief Selects all commission rate rules and populates the provided map
	 *
	 * @param commissionRateRules Reference to map to store all rules indexed by rule ID
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves all commission rate rule configurations from the database
	 * @note Populates the commissionRateRules map with rule ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, CommissionRateRule> &commissionRateRules);
};