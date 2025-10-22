#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "MarketResponse.h"

/**
 * @class MarketResponse_dbt
 * @brief Database table class for MarketResponse entity operations
 * 
 * This class provides database operations for the MarketResponse entity including
 * insertion operations for market response data. It extends DatabaseTable to
 * provide specific functionality for market response database operations.
 * 
 * Features:
 * - Market response insertion operations
 * - Exchange response data storage
 * - Order execution tracking
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides market response-specific database functionality
 * @note Supports exchange response data storage
 */
class MarketResponse_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for MarketResponse_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	MarketResponse_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Inserts a new market response record
	 * 
	 * @param marketResponse Reference to the MarketResponse object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts market response data from exchange APIs
	 * @note Stores order execution and response information
	 * @note Returns false if insertion fails
	 */
	bool Insert(MarketResponse& marketResponse);
};