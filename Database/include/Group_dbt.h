#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Group.h"


/**
 * @class Group_dbt
 * @brief Database table class for Group entity operations
 * 
 * This class provides database operations for the Group entity including
 * selection operations for group configurations. It extends DatabaseTable
 * to provide specific functionality for group-related database operations.
 * 
 * Features:
 * - Group selection operations
 * - Individual and bulk group selection
 * - Group configuration management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides group-specific database functionality
 * @note Supports both individual and bulk group operations
 */
class Group_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Group_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Group_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Selects a specific group by ID
	 * 
	 * @param Groups Reference to map to store the selected group
	 * @param Group_id ID of the group to select
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves specific group configuration from the database
	 * @note Populates the Groups map with the selected group
	 * @note Returns false if group not found
	 */
	bool Select(map<int, Group> &Groups, int Group_id);
	
	/**
	 * @brief Selects all groups and populates the provided map
	 * 
	 * @param Groups Reference to map to store all groups indexed by group ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all group configurations from the database
	 * @note Populates the Groups map with group ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, Group> &Groups);
};