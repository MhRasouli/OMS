/**
 * @file LastTradePriceGenerator_dbt.h
 * @brief Database table class for LastTradePriceGenerator entity operations
 *
 * This file contains the LastTradePriceGenerator_dbt class which provides
 * database operations for last trade price generator configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "LastTradePriceGenerator.h"

/**
 * @class LastTradePriceGenerator_dbt
 * @brief Database table class for LastTradePriceGenerator entity operations
 *
 * This class provides database operations for the LastTradePriceGenerator entity
 * including selection operations for last trade price generator configurations.
 * It extends DatabaseTable to provide specific functionality for last trade price
 * generator database operations.
 *
 * Features:
 * - Last trade price generator selection operations
 * - Individual and bulk generator selection
 * - Generator configuration management
 *
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides last trade price generator-specific database functionality
 * @note Supports both individual and bulk generator operations
 */
class LastTradePriceGenerator_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for LastTradePriceGenerator_dbt
	 *
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	LastTradePriceGenerator_dbt(DatabaseHandler* databaseHandler);

	/**
	 * @brief Selects a specific last trade price generator by ID
	 *
	 * @param lastTradePriceGenerators Reference to map to store the selected generator
	 * @param lastTradePriceGenerator_id ID of the last trade price generator to select
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves specific last trade price generator configuration from the database
	 * @note Populates the lastTradePriceGenerators map with the selected generator
	 * @note Returns false if generator not found
	 */
	bool Select(map<int, LastTradePriceGenerator>& lastTradePriceGenerators,int lastTradePriceGenerator_id);

	/**
	 * @brief Selects all last trade price generators and populates the provided map
	 *
	 * @param lastTradePriceGenerators Reference to map to store all generators indexed by generator ID
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves all last trade price generator configurations from the database
	 * @note Populates the lastTradePriceGenerators map with generator ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int,LastTradePriceGenerator>& lastTradePriceGenerators);
};