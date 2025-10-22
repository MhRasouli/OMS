#include "BuyLimitation_dbt.h"

BuyLimitation_dbt::BuyLimitation_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "buyLimitation")
{
}

bool BuyLimitation_dbt::Select(map<int, BuyLimitation> &buyLimitations, int BuyLimitation_id)
{
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "buyLimitation_id, seller_id, coin_id, max_sell_price, max_buy_qty, enable, commissionRate_id", "buyLimitation_id=%i", BuyLimitation_id))
        {
            if (table_data.size() > 0)
            {
                BuyLimitation result;
                result.buyLimitation_id = atoi(table_data[0][0].c_str());
                result.seller_id = atoi(table_data[0][1].c_str());
                result.coin_id=atoi(table_data[0][2].c_str());
                result.max_sell_price = atof(table_data[0][3].c_str());
                result.max_buy_qty = atof(table_data[0][4].c_str());
                result.enable = atoi(table_data[0][5].c_str()) == 0 ? false : true;
                result.commissionRate_id = atoi(table_data[0][6].c_str());

                if (buyLimitations.find(result.buyLimitation_id) == buyLimitations.end())
                {
                    buyLimitations[result.buyLimitation_id] = result;
                }
                else
                {
                    buyLimitations[result.buyLimitation_id].seller_id = result.seller_id;
                    buyLimitations[result.buyLimitation_id].coin_id = result.coin_id;
                    buyLimitations[result.buyLimitation_id].max_sell_price = result.max_sell_price;
                    buyLimitations[result.buyLimitation_id].max_buy_qty = result.max_buy_qty;
                    buyLimitations[result.buyLimitation_id].enable = result.enable;
                    buyLimitations[result.buyLimitation_id].commissionRate_id=result.commissionRate_id;
                }

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", BuyLimitation_id, tableName.c_str());
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
bool BuyLimitation_dbt::SelectAll(map<int, BuyLimitation> &buyLimitations)
{
    buyLimitations.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "buyLimitation_id, seller_id, coin_id, max_sell_price, max_buy_qty, enable, commissionRate_id"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    BuyLimitation result;
                    result.buyLimitation_id = atoi(table_data[i][0].c_str());
                    result.seller_id = atoi(table_data[i][1].c_str());
                    result.coin_id=atoi(table_data[i][2].c_str());
                    result.max_sell_price = atof(table_data[i][3].c_str());
                    result.max_buy_qty = atof(table_data[i][4].c_str());
                    result.enable = atoi(table_data[i][5].c_str()) == 0 ? false : true;
                    result.commissionRate_id=atoi(table_data[i][6].c_str());

                    buyLimitations[result.buyLimitation_id] = result;
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