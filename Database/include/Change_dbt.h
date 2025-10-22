#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Change.h"

/**
 * @class Change_dbt
 * @brief Database table class for Change entity operations
 * 
 * This class provides detection of all changes at all tables.
 * If any record in a table is changed (Insert, Update or Delete)
 * ID of the changed record will be registered into Change table by the trigger of the table
 * OMS is checking the Change table frequently, if there is any new record in the Cahnge table,
 * OMS will load this change into its memory and, 
 * after the memory update, OMS will remove the record from the Change table
 * 
 * Features:
 * - Tracking the changes in all tables
 * - Checking the changes frequently
 * - Deleting the change record after updating the related OMS memory maps
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides change-specific database functionality
 * @note Supports change tracking and cleanup operations
 */
class Change_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Change_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Change_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects all changes and populates the provided map
	 * 
	 * @param changes Reference to map to store all changes indexed by change ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all changes from the database
	 * @note Populates the changes map with change ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, Change>& changes);
	
	/**
	 * @brief Deletes a specific change by ID
	 * 
	 * @param Change_id ID of the change (Insert, Update or Delete)
	 * @return bool True if deletion was successful, false otherwise
	 * 
	 * @note If the new change is Delete, it is true, else the new change is Insert or Update
	 * @note It is set false if the new change is Insert or Update
	 */
	bool Delete(int Change_id);
};