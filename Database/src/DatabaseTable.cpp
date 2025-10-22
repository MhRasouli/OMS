/**
 * @file DatabaseTable.cpp
 * @brief Implementation of the DatabaseTable base class for database operations
 * 
 * This file contains the implementation of the DatabaseTable class which provides
 * common database operations including CRUD operations, parameterized queries,
 * and transaction management for all database table implementations.
 */

#include "DatabaseTable.h"

/*
bool DatabaseTable::LoadAllData(string WhereCondition)
{
	bool res = false;
	try
	{
		mut_allDataInTable->lock_shared();
		if (res = ReadTable(allDataInTable, "*", WhereCondition.c_str()))
		{
			Logger::logger.log_special(LogLocation, false, "The data of table %s loaded", tableName.c_str());
		}
		mut_allDataInTable->unlock_shared();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "table name: %s .Error: %s", tableName.c_str(), ex.what());
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "table name: %s .Error: Unknown", tableName.c_str());
	}
	return res;
}
*/

/**
 * @brief Constructor for DatabaseTable
 * 
 * @param _DatabaseHandler Pointer to the database handler for connection management
 * @param TableName Name of the database table
 * 
 * @note Initializes the database table with handler and table name
 * @note Sets up the foundation for database operations
 */
DatabaseTable::DatabaseTable(DatabaseHandler *_DatabaseHandler, string TableName)
{
	databaseHandler = _DatabaseHandler;
	tableName = TableName;
}

/**
 * @brief Executes a raw SQL query
 * 
 * @param SqlQuery The SQL query string to execute
 * @return bool True if query executed successfully, false otherwise
 * 
 * @note Use with caution - no parameter binding for this method
 * @note Suitable for DDL operations or complex queries
 * @note Thread-safe operation with unique_lock
 * @note Handles various exception types and logs errors
 * @note Calls OnConnectionLost() callback on database errors
 */
bool DatabaseTable::ExecQuery(string SqlQuery)
{
	unique_lock lock(*databaseHandler->mut_DB);
	bool res = true;
	try
	{
		databaseHandler->DB->execute(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Query execution on table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	return res;
}

bool DatabaseTable::DeleteTable(const char *WhereCondition, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);

	bool res = false;

	std::string SqlQuery = "DELETE FROM ";
	SqlQuery += tableName;
	if (WhereCondition != NULL)
	{
		char buffer[4096];
		va_list arg;
		va_start(arg, WhereCondition);
		vsnprintf(buffer, 4096, WhereCondition, arg);
		va_end(arg);

		SqlQuery += " WHERE ";
		SqlQuery += buffer;
	}

	// cout << SqlQuery<<endl;

	try
	{
		databaseHandler->DB->execute(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
		res = true;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "delete data from table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}

bool DatabaseTable::ReadTable_orderby(std::vector<std::vector<std::string>> &Result, std::string ColumnsNames, const char *Orderby, const char *WhereCondition, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);

	bool res = true;
	Result.clear();

	std::string SqlQuery = "SELECT ";
	SqlQuery += ColumnsNames + " FROM ";
	SqlQuery += tableName;
	if (WhereCondition != NULL)
	{
		char buffer[4096];
		va_list arg;
		va_start(arg, WhereCondition);
		vsnprintf(buffer, 4096, WhereCondition, arg);
		va_end(arg);

		SqlQuery += " WHERE ";
		SqlQuery += buffer;
	}
	if (Orderby != NULL)
	{
		SqlQuery += " ORDER BY ";
		SqlQuery += Orderby;
	}

	// cout << SqlQuery<<endl;

	try
	{
		sql::ResultSet *resultSet = databaseHandler->DB->executeQuery(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
		while (resultSet->next())
		{
			// Retrieving the ResultSetMetaData object
			sql::ResultSetMetaData *rsmd = resultSet->getMetaData();
			// getting the column type
			int colCount = rsmd->getColumnCount();
			std::vector<std::string> infoitem;
			for (int i = 1; i <= colCount; i++)
			{
				infoitem.push_back(resultSet->getString(i));
			}
			Result.push_back(infoitem);
		}
		resultSet->close();
		delete resultSet;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Read data from table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}

bool DatabaseTable::ReadTable(std::vector<std::vector<std::string>> &Result, std::string ColumnsNames, const char *WhereCondition, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);

	bool res = true;
	Result.clear();

	std::string SqlQuery = "SELECT ";
	SqlQuery += ColumnsNames + " FROM ";
	SqlQuery += tableName;
	if (WhereCondition != NULL)
	{
		char buffer[4096];
		va_list arg;
		va_start(arg, WhereCondition);
		vsnprintf(buffer, 4096, WhereCondition, arg);
		va_end(arg);

		SqlQuery += " WHERE ";
		SqlQuery += buffer;
	}

	// cout << SqlQuery<<endl;

	try
	{
		sql::ResultSet *resultSet = databaseHandler->DB->executeQuery(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
		while (resultSet->next())
		{
			// Retrieving the ResultSetMetaData object
			sql::ResultSetMetaData *rsmd = resultSet->getMetaData();
			// getting the column type
			int colCount = rsmd->getColumnCount();
			std::vector<std::string> infoitem;
			for (int i = 1; i <= colCount; i++)
			{
				infoitem.push_back(resultSet->getString(i));
			}
			Result.push_back(infoitem);
		}
		resultSet->close();
		delete resultSet;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Read data from table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}

int DatabaseTable::InsertTable_IfNotExist(string IfNotExist_WhereCondition, std::string ColumnsName, const char *Values, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);

	int res = -1;

	std::string SqlQuery = "CALL InsertGetID('";
	SqlQuery += databaseHandler->db_name;
	SqlQuery += "','";
	SqlQuery += tableName;
	SqlQuery += "',\"";
	SqlQuery += "IF NOT EXISTS (SELECT * FROM `";
	SqlQuery += tableName;
	SqlQuery += "` WHERE ";
	SqlQuery += IfNotExist_WhereCondition;

	SqlQuery += ") THEN INSERT INTO ";
	SqlQuery += tableName;
	SqlQuery += " (";
	SqlQuery += ColumnsName;
	SqlQuery += ") VALUES(";

	char buffer[4096];
	va_list arg;
	va_start(arg, Values);
	vsnprintf(buffer, 4096, Values, arg);
	va_end(arg);

	SqlQuery += buffer;
	SqlQuery += "); ";
	SqlQuery += "END IF;\",@";
	SqlQuery += tableName;
	SqlQuery += "_ID);";

	//	cout << SqlQuery<<endl;

	try
	{
		databaseHandler->DB->execute(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
		SqlQuery = "SELECT @";
		SqlQuery += tableName;
		SqlQuery += "_ID as id;";
		sql::ResultSet *resultSet = databaseHandler->DB->executeQuery(SqlQuery.c_str());
		while (resultSet->next())
		{
			// Retrieving the ResultSetMetaData object
			sql::ResultSetMetaData *rsmd = resultSet->getMetaData();
			// getting the column type
			int colCount = rsmd->getColumnCount();

			res = atoi(resultSet->getString(1).c_str());
			break;
		}
		resultSet->close();
		delete resultSet;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Insert data into table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}

int DatabaseTable::InsertTable(std::string ColumnsName, const char *Values, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);
	int res = -1;

	std::string SqlQuery = "INSERT INTO ";
	SqlQuery += tableName;
	SqlQuery += " (";
	SqlQuery += ColumnsName;
	SqlQuery += ") VALUES(";

	char buffer[4096];
	va_list arg;
	va_start(arg, Values);
	vsnprintf(buffer, 4096, Values, arg);
	va_end(arg);

	SqlQuery += buffer;
	SqlQuery += ");";

	try
	{
			databaseHandler->DB->execute(SqlQuery.c_str());
			std::string getIdQuery = "SELECT getAutoincrementID('";
			getIdQuery += tableName;
			getIdQuery += "');";
			sql::ResultSet *resultSet = databaseHandler->DB->executeQuery(getIdQuery.c_str());
			while (resultSet->next())
			{
				sql::ResultSetMetaData *rsmd = resultSet->getMetaData();
				int colCount = rsmd->getColumnCount();

				res = atoi(resultSet->getString(1).c_str());
				break;
			}
			resultSet->close();
			delete resultSet;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Insert data into table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}

bool DatabaseTable::UpdateTable(std::string WhereCondition, const char *ColumnName_Value, ...)
{
	unique_lock lock(*databaseHandler->mut_DB);

	bool res = false;

	std::string SqlQuery = "UPDATE ";
	SqlQuery += tableName;
	SqlQuery += " SET ";

	char buffer[4096];
	va_list arg;
	va_start(arg, ColumnName_Value);
	vsnprintf(buffer, 4096, ColumnName_Value, arg);
	va_end(arg);

	SqlQuery += buffer;
	SqlQuery += " WHERE ";
	SqlQuery += WhereCondition;

	// cout << SqlQuery<<endl;
	try
	{
		databaseHandler->DB->execute(SqlQuery.c_str());
		Logger::logger.log_special_if_verb(LogLocation, false, "Query executed: %s", SqlQuery.c_str());
		res = true;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Update data of table %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", tableName.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "%s", ex.what());
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Unknown");
		Logger::logger.log_special(LogLocation, false, "%s", SqlQuery.c_str());
		res = false;

		databaseHandler->OnConnectionLost();
	}

	return res;
}