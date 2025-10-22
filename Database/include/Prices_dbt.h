/**
 * @file Prices_dbt.h
 * @brief Database table class for Prices entity operations
 *
 * This file contains the Prices_dbt class which provides
 * database operations for price configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Prices.h"

/**
 * @class Prices_dbt
 * @brief Database table class for Prices entity operations
 *
 * This class provides database operations for the Prices entity including
 * update operations for cryptocurrency price data. It extends DatabaseTable
 * to provide specific functionality for price-related database operations.
 *
 * Features:
 * - Price update operations
 * - Bid/ask price updates
 * - Last trade price (LTP) updates
 * - Real-time price data management
 *
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides price-specific database functionality
 * @note Supports real-time price data updates
 */
class Prices_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Prices_dbt
	 *
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Prices_dbt(DatabaseHandler* databaseHandler);

	/**
	 * @brief Updates price data for a cryptocurrency
	 *
	 * @param prices Prices object containing updated price information
	 * @return bool True if update was successful, false otherwise
	 *
	 * @note Updates comprehensive price data including bid, ask, and last trade price
	 * @note Modifies existing price records in the database
	 * @note Returns false if update fails
	 */
	bool Update(const Prices prices);

	/**
	 * @brief Updates bid and ask prices for a cryptocurrency
	 *
	 * @param prices Prices object containing updated bid/ask information
	 * @return bool True if update was successful, false otherwise
	 *
	 * @note Updates only bid and ask price data
	 * @note Preserves other price information
	 * @note Returns false if update fails
	 */
	bool Update_bid_ask(const Prices prices);

	/**
	 * @brief Updates last trade price (LTP) for a cryptocurrency
	 *
	 * @param prices Prices object containing updated last trade price information
	 * @return bool True if update was successful, false otherwise
	 *
	 * @note Updates only the last trade price data
	 * @note Preserves other price information
	 * @note Returns false if update fails
	 */
	bool Update_ltp(const Prices prices);
};