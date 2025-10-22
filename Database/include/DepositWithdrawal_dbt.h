#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "DepositWithdrawal.h"

/**
 * @class DepositWithdrawal_dbt
 * @brief Database table class for DepositWithdrawal entity operations
 * 
 * This class provides comprehensive database operations for the DepositWithdrawal
 * entity including selection, insertion, and update operations. It extends
 * DatabaseTable to provide specific functionality for deposit/withdrawal
 * database operations.
 * 
 * Features:
 * - Deposit/withdrawal selection operations
 * - Individual and bulk deposit/withdrawal selection
 * - Deposit/withdrawal insertion and update operations
 * - Transaction management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides comprehensive deposit/withdrawal database functionality
 * @note Supports full CRUD operations for deposit/withdrawal transactions
 */
class DepositWithdrawal_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for DepositWithdrawal_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	DepositWithdrawal_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific deposit/withdrawal by ID
	 * 
	 * @param depositWithdrawals Reference to map to store the selected deposit/withdrawal
	 * @param depositWithdrawal_id ID of the deposit/withdrawal to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific deposit/withdrawal transaction from the database
	 * @note Populates the depositWithdrawals map with the selected transaction
	 * @note Returns false if deposit/withdrawal not found
	 */
	bool Select(map<int,DepositWithdrawal>& depositWithdrawals,int depositWithdrawal_id);
	
	/**
	 * @brief Selects all deposits/withdrawals and populates the provided map
	 * 
	 * @param depositWithdrawals Reference to map to store all deposits/withdrawals indexed by ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all deposit/withdrawal transactions from the database
	 * @note Populates the depositWithdrawals map with transaction ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int,DepositWithdrawal>& depositWithdrawals);
	
	/**
	 * @brief Updates an existing deposit/withdrawal record
	 * 
	 * @param depositWithdrawal DepositWithdrawal object containing updated information
	 * @return bool True if update was successful, false otherwise
	 * 
	 * @note Updates deposit/withdrawal transaction in the database
	 * @note Modifies existing transaction information
	 * @note Returns false if update fails
	 */
	bool Update(DepositWithdrawal depositWithdrawal);
	
	/**
	 * @brief Inserts a new deposit/withdrawal record
	 * 
	 * @param depositWithdrawal Reference to the DepositWithdrawal object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts new deposit/withdrawal transaction into the database
	 * @note Creates new transaction record
	 * @note Returns false if insertion fails
	 */
	bool Insert(DepositWithdrawal &depositWithdrawal);
};