#include "Market_dbt.h"

Market_dbt::Market_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "market")
{
}
bool Market_dbt::Select(map<int, Market> &markets, int Market_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "market_id, name, enable, host_trade, port_trade, host_feedPrice, port_feedPrice, username_privateKey, password_securityKey, passphrase, tradeIsActive, feedPricesIsValidUntil_sec, minimumTradePrice_usd, accountID_TradeWith_LessThanMinTradePrice, accountID_TradeWith_TradeIsNotActive, accountID_MarketWalletBalance, checkMarketWalletBalanceEvery_sec", "market_id=%i", Market_id))
        {
            if (table_data.size() > 0)
            {
                Market result;
                int i=0;
                result.market_id = atoi(table_data[0][i++].c_str());
                result.name = table_data[0][i++];
                result.enable = atoi(table_data[0][i++].c_str())==0 ? false : true;
                result.host_trade = table_data[0][i++];
                result.port_trade = atoi(table_data[0][i++].c_str());
                result.host_feedPrice = table_data[0][i++];
                result.port_feedPrice = atoi(table_data[0][i++].c_str());
                result.username_privateKey = table_data[0][i++];
                result.password_securityKey = table_data[0][i++];
                result.passphrase=table_data[0][i++];
                result.tradeIsActive = atoi(table_data[0][i++].c_str()) == 0 ? false : true;
                result.feedPricesIsValidUntil_sec = atoi(table_data[0][i++].c_str());
                result.minimumTradePrice_usd = atof(table_data[0][i++].c_str());
                result.accountID_TradeWith_LessThanMinTradePrice = atoi(table_data[0][i++].c_str());
                result.accountID_TradeWith_TradeIsNotActive = atoi(table_data[0][i++].c_str());
                result.accountID_MarketWalletBalance = atoi(table_data[0][i++].c_str());
                result.checkMarketWalletBalanceEvery_sec=atoi(table_data[0][i++].c_str());

                if (markets.find(result.market_id) == markets.end())
                {
                    markets[result.market_id] = result;
                }
                else
                {
                    markets[result.market_id].name = result.name;
                    markets[result.market_id].enable = result.enable;
                    markets[result.market_id].host_trade = result.host_trade;
                    markets[result.market_id].port_trade = result.port_trade;
                    markets[result.market_id].host_feedPrice = result.host_feedPrice;
                    markets[result.market_id].port_feedPrice = result.port_feedPrice;
                    markets[result.market_id].username_privateKey = result.username_privateKey;
                    markets[result.market_id].password_securityKey = result.password_securityKey;
                    markets[result.market_id].passphrase=result.passphrase;
                    markets[result.market_id].tradeIsActive = result.tradeIsActive;
                    markets[result.market_id].feedPricesIsValidUntil_sec = result.feedPricesIsValidUntil_sec;
                    markets[result.market_id].minimumTradePrice_usd = result.minimumTradePrice_usd;
                    markets[result.market_id].accountID_TradeWith_LessThanMinTradePrice = result.accountID_TradeWith_LessThanMinTradePrice;
                    markets[result.market_id].accountID_TradeWith_TradeIsNotActive = result.accountID_TradeWith_TradeIsNotActive;
                    markets[result.market_id].accountID_MarketWalletBalance = result.accountID_MarketWalletBalance;
                    markets[result.market_id].checkMarketWalletBalanceEvery_sec=result.checkMarketWalletBalanceEvery_sec;
                }

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", Market_id, tableName.c_str());
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
bool Market_dbt::SelectAll(map<int, Market> &markets)
{
    markets.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "market_id, name, enable, host_trade, port_trade, host_feedPrice, port_feedPrice, username_privateKey, password_securityKey, passphrase, tradeIsActive, feedPricesIsValidUntil_sec, minimumTradePrice_usd, accountID_TradeWith_LessThanMinTradePrice, accountID_TradeWith_TradeIsNotActive, accountID_MarketWalletBalance, checkMarketWalletBalanceEvery_sec"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    Market result;
                    int j=0;
                    result.market_id = atoi(table_data[i][j++].c_str());
                    result.name = table_data[i][j++];
                    result.enable = atoi(table_data[i][j++].c_str())==0 ? false : true;
                    result.host_trade = table_data[i][j++];
                    result.port_trade = atoi(table_data[i][j++].c_str());
                    result.host_feedPrice = table_data[i][j++];
                    result.port_feedPrice = atoi(table_data[i][j++].c_str());
                    result.username_privateKey = table_data[i][j++];
                    result.password_securityKey = table_data[i][j++];
                    result.passphrase = table_data[i][j++];
                    result.tradeIsActive = atoi(table_data[i][j++].c_str()) == 0 ? false : true;
                    result.feedPricesIsValidUntil_sec = atoi(table_data[i][j++].c_str());
                    result.minimumTradePrice_usd = atof(table_data[i][j++].c_str());
                    result.accountID_TradeWith_LessThanMinTradePrice = atoi(table_data[i][j++].c_str());
                    result.accountID_TradeWith_TradeIsNotActive = atoi(table_data[i][j++].c_str());
                    result.accountID_MarketWalletBalance = atoi(table_data[i][j++].c_str());
                    result.checkMarketWalletBalanceEvery_sec=atoi(table_data[i][j++].c_str());

                    markets[result.market_id]=result;
                }
                res = true;
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

