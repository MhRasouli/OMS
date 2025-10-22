#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "TradeRequestGenerator.h"

/**
 * @class TradeRequestGenerator_dbt
 * @brief Database table class for TradeRequestGenerator entity operations
 * 
 * This class provides database operations for the TradeRequestGenerator entity
 * including selection operations for trade request generator configurations.
 * It extends DatabaseTable to provide specific functionality for trade request
 * generator database operations.
 * 
 * Features:
 * - Trade request generator selection operations
 * - Individual and bulk generator selection
 * - Generator configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides trade request generator-specific database functionality
 * @note Supports both individual and bulk generator operations
 */
class TradeRequestGenerator_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for TradeRequestGenerator_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	TradeRequestGenerator_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects all trade request generators and populates the provided map
	 * 
	 * @param tradeRequestGenerators Reference to map to store all generators indexed by generator ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all trade request generators from the database
	 * @note Populates the tradeRequestGenerators map with generator ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, TradeRequestGenerator>& tradeRequestGenerators);
	
	/**
	 * @brief Selects a specific trade request generator by ID
	 * 
	 * @param tradeRequestGenerators Reference to map to store the selected generator
	 * @param tradeRequestGenerators_id ID of the generator to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific trade request generator from the database
	 * @note Populates the tradeRequestGenerators map with the selected generator
	 * @note Returns false if generator not found
	 */
	bool Select(map<int, TradeRequestGenerator>& tradeRequestGenerators, int tradeRequestGenerators_id);
};