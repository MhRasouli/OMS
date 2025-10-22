#include "LastTradePriceGenerator_dbt.h"

LastTradePriceGenerator_dbt::LastTradePriceGenerator_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "lastTradePriceGenerator")
{
}
bool LastTradePriceGenerator_dbt::Select(map<int, LastTradePriceGenerator> &lastTradePriceGenerators, int lastTradePriceGenerator_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "lastTradePriceGenerator_id, \
        priceAreaAroundAvgOfAskBid_min_perc, priceAreaAroundAvgOfAskBid_max_perc, \
        generateDelay_min_sec, generateDelay_max_sec",
                      "lastTradePriceGenerator_id,=%i", lastTradePriceGenerator_id))
        {
            if (table_data.size() > 0)
            {
                LastTradePriceGenerator result;
                int j = 0;

                result.lastTradePriceGenerator_id = atoi(table_data[0][j++].c_str());
                result.priceAreaAroundAvgOfAskBid_min_perc = atof(table_data[0][j++].c_str())/100.0;
                result.priceAreaAroundAvgOfAskBid_max_perc = atof(table_data[0][j++].c_str())/100.0;
                result.generateDelay_min_sec = atof(table_data[0][j++].c_str());
                result.generateDelay_max_sec = atof(table_data[0][j++].c_str());

                lastTradePriceGenerators[result.lastTradePriceGenerator_id] = result;

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", lastTradePriceGenerator_id, tableName.c_str());
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
bool LastTradePriceGenerator_dbt::SelectAll(map<int, LastTradePriceGenerator> &lastTradePriceGenerators)
{
    lastTradePriceGenerators.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "lastTradePriceGenerator_id, \
        priceAreaAroundAvgOfAskBid_min_perc, priceAreaAroundAvgOfAskBid_max_perc, \
        generateDelay_min_sec, generateDelay_max_sec"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    LastTradePriceGenerator result;
                    int j = 0;

                    result.lastTradePriceGenerator_id = atoi(table_data[i][j++].c_str());
                    result.priceAreaAroundAvgOfAskBid_min_perc = atof(table_data[i][j++].c_str())/100.0;
                    result.priceAreaAroundAvgOfAskBid_max_perc = atof(table_data[i][j++].c_str())/100.0;
                    result.generateDelay_min_sec = atof(table_data[i][j++].c_str());
                    result.generateDelay_max_sec = atof(table_data[i][j++].c_str());

                    lastTradePriceGenerators[result.lastTradePriceGenerator_id] = result;

                    res = true;
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
