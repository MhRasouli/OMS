#include "GroupCoinMarketCommission_dbt.h"

GroupCoinMarketCommission_dbt::GroupCoinMarketCommission_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "`groupCoinMarketCommission`")
{
}

bool GroupCoinMarketCommission_dbt::Select(map<pair<int, int>, GroupCoinMarketCommission> &GCMCs, int GCMC_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;
        if (ReadTable(table_data, "groupCoinMarketCommission_id, group_id, coin_id, marketOrder_commissionRateRule_id, pendingOrder_commissionRateRule_id", "groupCoinMarketCommission_id=%i", GCMC_id))
        {
            if (table_data.size() > 0)
            {
                GroupCoinMarketCommission result;

                result.groupCoinMarketCommission_id = atoi(table_data[0][0].c_str());
                result.group_id = atoi(table_data[0][1].c_str());
                result.coin_id = atoi(table_data[0][2].c_str());
                result.marketOrder_commissionRateRule_id = atoi(table_data[0][3].c_str());
                result.pendingOrder_commissionRateRule_id = atoi(table_data[0][4].c_str());

                GCMCs[TwoKey(result.group_id, result.coin_id)] = result;

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", GCMC_id, tableName.c_str());
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

bool GroupCoinMarketCommission_dbt::SelectAll(map<pair<int, int>, GroupCoinMarketCommission> &GCMCs)
{
    GCMCs.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "groupCoinMarketCommission_id, group_id, coin_id, marketOrder_commissionRateRule_id, pendingOrder_commissionRateRule_id"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    GroupCoinMarketCommission result;

                    result.groupCoinMarketCommission_id = atoi(table_data[i][0].c_str());
                    result.group_id = atoi(table_data[i][1].c_str());
                    result.coin_id = atoi(table_data[i][2].c_str());
                    result.marketOrder_commissionRateRule_id = atoi(table_data[i][3].c_str());
                    result.pendingOrder_commissionRateRule_id = atoi(table_data[i][4].c_str());

                    GCMCs[TwoKey(result.group_id, result.coin_id)] = result;
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