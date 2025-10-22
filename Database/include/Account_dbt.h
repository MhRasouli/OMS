#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Account.h"

/**
 * @class Account_dbt
 * @brief Database table class for Account entity operations
 * 
 * This class provides database operations for the Account entity including
 * selection operations for individual accounts and all accounts. It extends
 * DatabaseTable to provide specific functionality for account-related database operations.
 * 
 * Features:
 * - Account selection operations
 * - Individual account retrieval
 * - Bulk account selection
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides account-specific database functionality
 * @note Supports both individual and bulk account operations
 */
class Account_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Account_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Account_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific account by ID
	 * 
	 * @param Account_id ID of the account to select
	 * @return Account Account object containing account information
	 * 
	 * @note Retrieves account information for the specified account ID
	 * @note Returns empty Account object if account not found
	 */
	Account Select(int Account_id);
	
	/**
	 * @brief Selects all accounts and populates the provided map
	 * 
	 * @param accounts Reference to map to store all accounts indexed by account ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all accounts from the database
	 * @note Populates the accounts map with account ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, Account>& accounts);
};