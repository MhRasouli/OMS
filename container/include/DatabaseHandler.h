#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct DatabaseHandler
 * @brief Data structure representing database connection and management
 * 
 * This structure contains database connection information and management
 * functionality including connection details, statement handling, and
 * connection loss callback support.
 * 
 * Key Features:
 * - Database name and connection information
 * - SQL statement pointer for database operations
 * - Thread-safe mutex for database access
 * - Connection loss callback function
 * 
 * @note Contains database connection management
 * @note Supports thread-safe database operations
 * @note Includes connection loss handling
 */
struct DatabaseHandler
{
	string db_name;///< Name of the database
	sql::Statement* DB;///< Pointer to SQL statement object for database operations
	shared_mutex* mut_DB;///< Shared mutex for thread-safe database access
	std::function<void()> OnConnectionLost = {};///< Callback function for connection loss events
};