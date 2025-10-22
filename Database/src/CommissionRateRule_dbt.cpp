#include "CommissionRateRule_dbt.h"

CommissionRateRule_dbt::CommissionRateRule_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "commissionRateRule")
{
}
bool CommissionRateRule_dbt::Select(map<int, CommissionRateRule> &CommissionRateRules, int CommissionRateRule_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "commissionRateRule_id, commissionRate_id_market, commissionRate_id_broker, commissionRate_id_local, commissionSwap_perc", "CommissionRateRule_id=%i", CommissionRateRule_id))
        {
            if (table_data.size() > 0)
            {
                CommissionRateRule result;
                result.commissionRateRule_id = atoi(table_data[0][0].c_str());
                result.commissionRate_id_market = atoi(table_data[0][1].c_str());
                result.commissionRate_id_broker = atoi(table_data[0][2].c_str());
                result.commissionRate_id_local = atoi(table_data[0][3].c_str());
                result.commissionSwap_perc = atof(table_data[0][4].c_str())/100.0;

                CommissionRateRules[CommissionRateRule_id] = result;

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", CommissionRateRule_id, tableName.c_str());
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
bool CommissionRateRule_dbt::SelectAll(map<int, CommissionRateRule> &CommissionRateRules)
{
    CommissionRateRules.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "commissionRateRule_id, commissionRate_id_market, commissionRate_id_broker, commissionRate_id_local, commissionSwap_perc"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    CommissionRateRule result;
                    result.commissionRateRule_id = atoi(table_data[i][0].c_str());
                    result.commissionRate_id_market = atoi(table_data[i][1].c_str());
                    result.commissionRate_id_broker = atoi(table_data[i][2].c_str());
                    result.commissionRate_id_local = atoi(table_data[i][3].c_str());
                    result.commissionSwap_perc = atof(table_data[i][4].c_str())/100.0;

                    CommissionRateRules[result.commissionRateRule_id] = result;
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