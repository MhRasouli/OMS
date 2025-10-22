#include "Account.h"
#include "Account_dbt.h"

Account_dbt::Account_dbt(DatabaseHandler* databaseHandler) : DatabaseTable(databaseHandler, "account")
{

}

bool Account_dbt::SelectAll(map<int, Account>& accounts)
{
	accounts.clear();
	bool res=false;
	try
	{
		std::vector<std::vector<std::string>> table_data;

		if (ReadTable(table_data, "account_id, balance_usdt, tradePrice_lastMonth_usdt, group_id"))
		{
			if (table_data.size() > 0)
			{
				for (int i = 0; i < table_data.size(); i++)
				{
					Account result;
					result.account_id = atoi(table_data[i][0].c_str());
					result.balance_usdt = atof(table_data[i][1].c_str());
					result.tradePrice_lastMonth_usdt=atof(table_data[i][2].c_str());
					result.group_id=atoi(table_data[i][3].c_str());
					accounts.insert(pair<int, Account>(result.account_id, result));
				}
				
			}
			else
			{
				Logger::logger.log_special(LogLocation, false, "Table %s is empty", tableName.c_str());
			}
		}
		else
		{
			Logger::logger.log_special(LogLocation, true, "select from table %s failed", tableName.c_str());
		}
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}

	return res;
}

Account Account_dbt::Select(int Account_id)
{
	Account result;
	try
	{
		std::vector<std::vector<std::string>> table_data;

		if (ReadTable(table_data, "balance_usdt, tradePrice_lastMonth_usdt, group_id", "account_id=%i", Account_id))
		{
			if (table_data.size() > 0)
			{
				result.account_id = Account_id;
				result.balance_usdt = atof(table_data[0][0].c_str());
				result.tradePrice_lastMonth_usdt=atof(table_data[0][1].c_str());
				result.group_id = atoi(table_data[0][2].c_str());
			}
			else
			{
				Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", Account_id, tableName.c_str());
			}
		}
		else
		{
			Logger::logger.log_special(LogLocation, true, "select from table %s failed", tableName.c_str());
		}
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}

	return result;
}