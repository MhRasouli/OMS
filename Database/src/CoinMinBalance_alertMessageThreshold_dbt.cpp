#include "CoinMinBalance_alertMessageThreshold_dbt.h"

CoinMinBalance_alertMessageThreshold_dbt::CoinMinBalance_alertMessageThreshold_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "coinMinBalance_alertMessageThreshold")
{
}

bool CoinMinBalance_alertMessageThreshold_dbt::Select(map<int, CoinMinBalance_alertMessageThreshold> &coinMinBalances, int coinMinBalance_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "coinMinBalance_alertMessageThreshold_id, lessThanMinTradePrice, tradeIsNotActive, marketWalletBalance", "coinMinBalance_alertMessageThreshold_id=%i", coinMinBalance_id))
        {
            if (table_data.size() > 0)
            {
                CoinMinBalance_alertMessageThreshold result;
                result.coinMinBalance_alertMessageThreshold_id = atoi(table_data[0][0].c_str());
                result.lessThanMinTradePrice = atof(table_data[0][1].c_str());
                result.tradeIsNotActive = atof(table_data[0][2].c_str());
                result.marketWalletBalance = atof(table_data[0][3].c_str());

                if (coinMinBalances.find(result.coinMinBalance_alertMessageThreshold_id) == coinMinBalances.end())
                {
                    coinMinBalances[result.coinMinBalance_alertMessageThreshold_id] = result;
                }
                else
                {
                    coinMinBalances[result.coinMinBalance_alertMessageThreshold_id].lessThanMinTradePrice = result.lessThanMinTradePrice;
                    coinMinBalances[result.coinMinBalance_alertMessageThreshold_id].marketWalletBalance= result.marketWalletBalance;
                    coinMinBalances[result.coinMinBalance_alertMessageThreshold_id].tradeIsNotActive = result.tradeIsNotActive;
                }

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", coinMinBalance_id, tableName.c_str());
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

bool CoinMinBalance_alertMessageThreshold_dbt::SelectAll(map<int, CoinMinBalance_alertMessageThreshold> &coinMinBalances)
{
    coinMinBalances.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "coinMinBalance_alertMessageThreshold_id, lessThanMinTradePrice, tradeIsNotActive, marketWalletBalance"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    CoinMinBalance_alertMessageThreshold result;
                    result.coinMinBalance_alertMessageThreshold_id = atoi(table_data[i][0].c_str());
                    result.lessThanMinTradePrice = atof(table_data[i][1].c_str());
                    result.tradeIsNotActive = atof(table_data[i][2].c_str());
                    result.marketWalletBalance = atof(table_data[i][3].c_str());

                    coinMinBalances[result.coinMinBalance_alertMessageThreshold_id] = result;
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