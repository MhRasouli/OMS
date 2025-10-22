/**
 * @file GroupCoinMarketCommission_dbt.h
 * @brief Database table class for GroupCoinMarketCommission entity operations
 *
 * This file contains the GroupCoinMarketCommission_dbt class which provides
 * database operations for group-coin-market commission configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "GroupCoinMarketCommission.h"

/**
 * @class GroupCoinMarketCommission_dbt
 * @brief Database table class for GroupCoinMarketCommission entity operations
 *
 * This class provides database operations for the GroupCoinMarketCommission entity
 * including selection operations for group-coin-market commission configurations.
 * It extends DatabaseTable to provide specific functionality for group-coin-market
 * commission database operations.
 *
 * Features:
 * - Group-coin-market commission selection operations
 * - Individual and bulk commission selection
 * - Commission configuration management
 *
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides group-coin-market commission-specific database functionality
 * @note Supports both individual and bulk commission operations
 */
class GroupCoinMarketCommission_dbt : public DatabaseTable
{
public:
	/**
	 * @brief Constructor for GroupCoinMarketCommission_dbt
	 *
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	GroupCoinMarketCommission_dbt(DatabaseHandler *databaseHandler);

	/**
	 * @brief Selects a specific group-coin-market commission by ID
	 *
	 * @param GCMCs Reference to map to store the selected commission
	 * @param GCMC_id ID of the group-coin-market commission to select
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves specific group-coin-market commission configuration from the database
	 * @note Populates the GCMCs map with the selected commission
	 * @note Returns false if commission not found
	 */
	bool Select(map<pair<int, int>, GroupCoinMarketCommission> &GCMCs, int GCMC_id);

	/**
	 * @brief Selects all group-coin-market commissions and populates the provided map
	 *
	 * @param GCMCs Reference to map to store all commissions indexed by group-coin-market pair
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves all group-coin-market commission configurations from the database
	 * @note Populates the GCMCs map with group-coin-market pair as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<pair<int, int>, GroupCoinMarketCommission> &GCMCs);
};