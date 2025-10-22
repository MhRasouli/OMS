#include "StairCommission.h"
#include "StairCommission_dbt.h"

StairCommission_dbt::StairCommission_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "stairCommission")
{
}

bool StairCommission_dbt::SelectAll(list<StairCommission> &stairCommissions)
{
    stairCommissions.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable_orderby(table_data, "stairCommission_id, marketOrder_commission_perc, pendingOrder_commission_perc, maxLastMonthTradePrice_usdt", "maxLastMonthTradePrice_usdt"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    StairCommission result;
                    result.stairCommission_id = atoi(table_data[i][0].c_str());
                    result.marketOrder_commission_perc = atof(table_data[i][1].c_str()) / 100.0;
                    result.pendingOrder_commission_perc = atof(table_data[i][2].c_str()) / 100.0;
                    result.maxLastMonthTradePrice_usdt = atof(table_data[i][3].c_str());

                    stairCommissions.push_back(result);
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

bool StairCommission_dbt::Select(list<StairCommission> &stairCommissions, int stairCommission_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "stairCommission_id, marketOrder_commission_perc, pendingOrder_commission_perc, maxLastMonthTradePrice_usdt", "stairCommission_id=%i", stairCommission_id))
        {
            if (table_data.size() > 0)
            {
                StairCommission result;
                result.stairCommission_id = atoi(table_data[0][0].c_str());
                result.marketOrder_commission_perc = atof(table_data[0][1].c_str()) / 100.0;
                result.pendingOrder_commission_perc = atof(table_data[0][2].c_str()) / 100.0;
                result.maxLastMonthTradePrice_usdt = atof(table_data[0][3].c_str());

                bool strComm_found = false;
                for (StairCommission &strComm : stairCommissions)
                {
                    if (strComm.stairCommission_id == result.stairCommission_id)
                    {
                        strComm.marketOrder_commission_perc = result.marketOrder_commission_perc;
                        strComm.pendingOrder_commission_perc = result.pendingOrder_commission_perc;

                        strComm.maxLastMonthTradePrice_usdt = result.maxLastMonthTradePrice_usdt;
                        strComm_found = true;
                        break;
                    }
                }

                if (!strComm_found)
                {
                    stairCommissions.push_back(result);
                }

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", stairCommission_id, tableName.c_str());
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