#include "CommissionRate_dbt.h"

CommissionRate_dbt::CommissionRate_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "commissionRate")
{
}
bool CommissionRate_dbt::Select(map<int, CommissionRate> &commissionRates, int commissionRate_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "commissionRate_id, buyer_price_perc, buyer_quantity_perc, buyer_commission_perc, seller_price_perc, seller_quantity_perc, seller_commission_perc, buyer_min_commission_usdt, seller_min_commission_usdt", "commissionRate_id=%i", commissionRate_id))
        {
            if (table_data.size() > 0)
            {
                CommissionRate result;
                result.commissionRate_id = atoi(table_data[0][0].c_str());
                result.buyer_price_perc = atof(table_data[0][1].c_str()) / 100.0;
                result.buyer_quantity_perc = atof(table_data[0][2].c_str()) / 100.0;
                result.buyer_commission_perc = atof(table_data[0][3].c_str()) / 100.0;
                result.seller_price_perc = atof(table_data[0][4].c_str()) / 100.0;
                result.seller_quantity_perc = atof(table_data[0][5].c_str()) / 100.0;
                result.seller_commission_perc = atof(table_data[0][6].c_str()) / 100.0;
                result.buyer_min_commission_usdt = atof(table_data[0][7].c_str());
                result.seller_min_commission_usdt = atof(table_data[0][8].c_str());

                commissionRates[commissionRate_id] = result;

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", commissionRate_id, tableName.c_str());
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
bool CommissionRate_dbt::SelectAll(map<int, CommissionRate> &commissionRates)
{
    commissionRates.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "commissionRate_id, buyer_price_perc, buyer_quantity_perc, buyer_commission_perc, seller_price_perc, seller_quantity_perc, seller_commission_perc, buyer_min_commission_usdt, seller_min_commission_usdt"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    CommissionRate result;
                    result.commissionRate_id = atoi(table_data[i][0].c_str());
                    result.buyer_price_perc = atof(table_data[i][1].c_str()) / 100.0;
                    result.buyer_quantity_perc = atof(table_data[i][2].c_str()) / 100.0;
                    result.buyer_commission_perc = atof(table_data[i][3].c_str()) / 100.0;
                    result.seller_price_perc = atof(table_data[i][4].c_str()) / 100.0;
                    result.seller_quantity_perc = atof(table_data[i][5].c_str()) / 100.0;
                    result.seller_commission_perc = atof(table_data[i][6].c_str()) / 100.0;
                    result.buyer_min_commission_usdt = atof(table_data[i][7].c_str());
                    result.seller_min_commission_usdt = atof(table_data[i][8].c_str());

                    commissionRates[result.commissionRate_id] = result;
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