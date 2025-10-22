#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Commission.h"

/**
 * @class Commission_dbt
 * @brief Database table class for Commission entity operations
 * 
 * This class provides database operations for the Commission entity including
 * insertion operations with trade price information. It extends DatabaseTable
 * to provide specific functionality for commission-related database operations.
 * 
 * Features:
 * - Commission insertion operations
 * - Trade price integration
 * - Coin-specific commission tracking
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides commission-specific database functionality
 * @note Supports trade price and coin association
 */
class Commission_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Commission_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Commission_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Inserts a new commission record with trade price information
	 * 
	 * @param commission Reference to the Commission object to insert
	 * @param coin_id ID of the coin associated with the commission
	 * @param tradePrice_buyer Trade price for the buyer
	 * @param tradePrice_seller Trade price for the seller
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts commission data with associated trade prices
	 * @note Links commission to specific coin and trade prices
	 * @note Returns false if insertion fails
	 */
	bool Insert(Commission& commission,int coin_id,double tradePrice_buyer, double tradePrice_seller);
};