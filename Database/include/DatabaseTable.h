#pragma once

#include "stdafx.h"
#include "Logger.h"
#include "Configuration.h"
#include "DatabaseHandler.h"

/**
 * @class DatabaseTable
 * @brief Base class for database table operations in the trading service
 * 
 * This class provides a foundation for database table operations including
 * CRUD (Create, Read, Update, Delete) operations with support for parameterized
 * queries and transaction management. It serves as the base class for all
 * specific database table implementations.
 * 
 * Features:
 * - Parameterized query support with variadic arguments
 * - Read operations with optional WHERE clauses and ORDER BY
 * - Insert operations with duplicate checking
 * - Update and delete operations
 * - Raw SQL query execution
 * - Database connection management through DatabaseHandler
 * 
 * @note Uses variadic arguments for flexible parameter binding
 * @note All operations are protected to be used by derived classes
 * @note Requires a DatabaseHandler instance for database connectivity
 */
class DatabaseTable
{
private:
	DatabaseHandler* databaseHandler;///< Pointer to database handler for connection management

public:
	/**
	 * @brief Constructor for DatabaseTable
	 * 
	 * Initializes the database table with a database handler and table name.
	 * 
	 * @param _DatabaseHandler Pointer to the database handler instance
	 * @param TableName Name of the database table
	 */
	DatabaseTable(DatabaseHandler* _DatabaseHandler,string TableName);

protected:
	//std::vector<std::vector<std::string>> allDataInTable;
	//bool LoadAllData(string WhereCondition = "");

	string tableName;///< Name of the database table

	/**
	 * @brief Reads data from the table with optional WHERE condition
	 * 
	 * @param Result Reference to store the query results
	 * @param ColumnsNames Comma-separated list of column names to select
	 * @param WhereCondition Optional WHERE clause (can be NULL)
	 * @param ... Variadic arguments for parameterized WHERE condition
	 * @return bool True if query executed successfully
	 * 
	 * @note Uses variadic arguments for parameter binding in WHERE clause
	 * @note Result is stored as vector of string vectors (rows x columns)
	 */
	bool ReadTable(std::vector<std::vector<std::string>>& Result, std::string ColumnsNames, const char* WhereCondition = NULL, ... );
	
	/**
	 * @brief Reads data from the table with ORDER BY and optional WHERE condition
	 * 
	 * @param Result Reference to store the query results
	 * @param ColumnsNames Comma-separated list of column names to select
	 * @param Orderby Optional ORDER BY clause (can be NULL)
	 * @param WhereCondition Optional WHERE clause (can be NULL)
	 * @param ... Variadic arguments for parameterized WHERE condition
	 * @return bool True if query executed successfully
	 * 
	 * @note Supports both ORDER BY and WHERE clauses with parameter binding
	 * @note Result is stored as vector of string vectors (rows x columns)
	 */
	bool ReadTable_orderby(std::vector<std::vector<std::string>> &Result, std::string ColumnsNames,const char *Orderby= NULL, const char *WhereCondition= NULL, ...);
	
	/**
	 * @brief Deletes records from the table with optional WHERE condition
	 * 
	 * @param WhereCondition Optional WHERE clause (can be NULL)
	 * @param ... Variadic arguments for parameterized WHERE condition
	 * @return bool True if delete operation executed successfully
	 * 
	 * @note If WHERE condition is NULL, deletes all records (use with caution)
	 * @note Uses variadic arguments for parameter binding
	 */
	bool DeleteTable(const char* WhereCondition = NULL, ...);
	
	/**
	 * @brief Executes a raw SQL query
	 * 
	 * @param SqlQuery The SQL query string to execute
	 * @return bool True if query executed successfully
	 * 
	 * @note Use with caution - no parameter binding for this method
	 * @note Suitable for DDL operations or complex queries
	 */
	bool ExecQuery(string SqlQuery);
	
	/**
	 * @brief Inserts a new record into the table
	 * 
	 * @param ColumnsNames Comma-separated list of column names
	 * @param Values Comma-separated list of values to insert
	 * @param ... Variadic arguments for additional values
	 * @return int Number of affected rows or -1 on error
	 * 
	 * @note Uses variadic arguments for flexible value insertion
	 * @note Returns the number of rows affected by the insert operation
	 */
	int  InsertTable(std::string ColumnsNames, const char* Values, ...);
	
	/**
	 * @brief Updates records in the table
	 * 
	 * @param WhereCondition WHERE clause to identify records to update
	 * @param ColumnName_Value Column name and value pairs for update
	 * @param ... Variadic arguments for additional column-value pairs
	 * @return bool True if update operation executed successfully
	 * 
	 * @note Uses variadic arguments for flexible column-value pair specification
	 * @note WHERE condition is required to prevent accidental mass updates
	 */
	bool UpdateTable(std::string WhereCondition, const char* ColumnName_Value, ...);
	
	/**
	 * @brief Inserts a record only if it doesn't already exist
	 * 
	 * @param IfNotExist_WhereCondition WHERE clause to check for existing records
	 * @param ColumnsName Comma-separated list of column names
	 * @param Values Comma-separated list of values to insert
	 * @param ... Variadic arguments for additional values
	 * @return int Number of affected rows or -1 on error
	 * 
	 * @note Performs existence check before insertion
	 * @note Uses variadic arguments for flexible value insertion
	 * @note Returns 0 if record already exists, 1 if inserted
	 */
	int  InsertTable_IfNotExist(string IfNotExist_WhereCondition, std::string ColumnsName, const char *Values, ...);
};
