/**
 * @file MarketResponseDetail_dbt.h
 * @brief Database table class for MarketResponseDetail entity operations
 * 
 * This file contains the MarketResponseDetail_dbt class which provides
 * database operations for detailed market response information.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "MarketResponseDetail.h"

/**
 * @class MarketResponseDetail_dbt
 * @brief Database table class for MarketResponseDetail entity operations
 * 
 * This class provides database operations for the MarketResponseDetail entity
 * including insertion operations for detailed market response data. It extends
 * DatabaseTable to provide specific functionality for market response detail
 * database operations.
 * 
 * Features:
 * - Market response detail insertion operations
 * - Detailed trade execution data storage
 * - Commission and trade ID tracking
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides market response detail-specific database functionality
 * @note Supports detailed trade execution data storage
 */
class MarketResponseDetail_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for MarketResponseDetail_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	MarketResponseDetail_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Inserts a new market response detail record
	 * 
	 * @param marketResponseDetail Reference to the MarketResponseDetail object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts detailed market response data from exchange APIs
	 * @note Stores individual trade execution details
	 * @note Returns false if insertion fails
	 */
	bool  Insert(MarketResponseDetail& marketResponseDetail);
};