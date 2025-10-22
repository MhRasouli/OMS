#include "Change.h"
#include "Change_dbt.h"

Change_dbt::Change_dbt(DatabaseHandler* databaseHandler) : DatabaseTable(databaseHandler, "`change`")
{

}

bool Change_dbt::Delete(int Change_id)
{
	bool result = false;
	try
	{
		char buffer[4096];
		sprintf(buffer, "DELETE FROM %s WHERE change_id=%i", tableName.c_str(), Change_id);
		ExecQuery(buffer);
		result = true;

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

bool Change_dbt::SelectAll(map<int, Change>& changes)
{
	changes.clear();
	bool res = false;
	try
	{
		std::vector<std::vector<std::string>> table_data;

		if (ReadTable(table_data, "change_id, deleted, \
		tradeRequest_id, wallet_id, account_id, coin_id, \
		tradeRequestGenerator_id, market_id, commissionRate_id, \
		stairCommission_id, depositWithdrawal_id, \
		coinMinBalance_alertMessageThreshold_id, group_id, buyLimitation_id, \
		groupCoinMarketCommission_id, tradeRequestRobot_id, lastTradePriceGenerator_id, commissionRateRule_id"))
		{
			for (int row = 0; row < table_data.size(); row++)
			{
				Change change;
				int j=0;
				change.change_id = atoi(table_data[row][j++].c_str());
				change.deleted = atoi(table_data[row][j++].c_str()) == 0 ? false : true;
				change.tradeRequest_id = atoi(table_data[row][j++].c_str());
				change.wallet_id = atoi(table_data[row][j++].c_str());
				change.account_id = atoi(table_data[row][j++].c_str());
				change.coin_id = atoi(table_data[row][j++].c_str());
				change.tradeRequestGenerator_id = atoi(table_data[row][j++].c_str());
				change.market_id = atoi(table_data[row][j++].c_str());
				change.commissionRate_id = atoi(table_data[row][j++].c_str());
				change.stairCommission_id = atoi(table_data[row][j++].c_str());
				change.depositWithdrawal_id = atoi(table_data[row][j++].c_str());
				change.coinMinBalance_alertMessageThreshold_id=atoi(table_data[row][j++].c_str());
				change.group_id=atoi(table_data[row][j++].c_str());
				change.buyLimitation_id=atoi(table_data[row][j++].c_str());
				change.groupCoinMarketCommission_id=atoi(table_data[row][j++].c_str());
				change.tradeRequestRobot_id=atoi(table_data[row][j++].c_str());
				change.lastTradePriceGenerator_id=atoi(table_data[row][j++].c_str());
				change.commissionRateRule_id=atoi(table_data[row][j++].c_str());
				
				changes[change.change_id]= change;
			}

			if (changes.size() > 0)
				res = true;
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

