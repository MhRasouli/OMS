#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "WalletHistory.h"
#include "UtilityTools.h"

/**
 * @class WalletHistory_dbt
 * @brief Database table class for WalletHistory entity operations
 * 
 * This class provides database operations for the WalletHistory entity including
 * insertion operations for wallet balance history tracking. It extends DatabaseTable
 * to provide specific functionality for wallet history database operations.
 * 
 * Features:
 * - Wallet history insertion operations
 * - Balance change tracking
 * - Trading service update flag support
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides wallet history-specific database functionality
 * @note Supports trading service update tracking
 */
class WalletHistory_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for WalletHistory_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	WalletHistory_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Inserts a new wallet history record
	 * 
	 * @param walletHistory Reference to the WalletHistory object to insert
	 * @param tradingService_updated Flag indicating if trading service was updated
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts wallet balance change history
	 * @note Tracks whether trading service was updated
	 * @note Returns false if insertion fails
	 */
	bool Insert(WalletHistory& walletHistory, bool tradingService_updated);
};