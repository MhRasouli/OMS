#include "DepositWithdrawal.h"
#include "DepositWithdrawal_dbt.h"

DepositWithdrawal_dbt::DepositWithdrawal_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "depositWithdrawal")
{
}

bool DepositWithdrawal_dbt::Insert(DepositWithdrawal &depositWithdrawal)
{
    bool result = false;
    try
    {
        int pk_id = 0;

if(depositWithdrawal.coin_id>0)
{
        pk_id = InsertTable(
            "account_id, coin_id, quantity_request, commission_quantity, `type`, status, time_request, network_fee_qty",
            "%i,%i,%.8f,%.8f,'%s',0,%s,%.8f",
            depositWithdrawal.account_id, depositWithdrawal.coin_id, depositWithdrawal.quantity_request, depositWithdrawal.commission_quantity, depositWithdrawal.type == type_DW_enum::Deposit ? "D" : "W", UtilityTools::Timestamp2DatetimeString(depositWithdrawal.time_request).c_str(),depositWithdrawal.network_fee_qty);
}
else if(depositWithdrawal.coin_id==0)
{
      pk_id = InsertTable(
            "account_id, quantity_request, commission_quantity, `type`, status, time_request, network_fee_qty",
            "%i,%.8f,%.8f,'%s',0,%s,%.8f",
            depositWithdrawal.account_id, depositWithdrawal.quantity_request, depositWithdrawal.commission_quantity, depositWithdrawal.type == type_DW_enum::Deposit ? "D" : "W", UtilityTools::Timestamp2DatetimeString(depositWithdrawal.time_request).c_str(),depositWithdrawal.network_fee_qty);
}
        if (pk_id > 0)
        {
            depositWithdrawal.depositWithdrawal_id = pk_id;
            result = true;
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

bool DepositWithdrawal_dbt::SelectAll(map<int,DepositWithdrawal> &depositWithdrawals)
{
    depositWithdrawals.clear();
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "depositWithdrawal_id, account_id, coin_id, quantity_request, commission_quantity, `type`, is_node", "status=0"))
        {
            if (table_data.size() > 0)
            {
                for (int i = 0; i < table_data.size(); i++)
                {
                    DepositWithdrawal result;
                    result.depositWithdrawal_id = atoi(table_data[i][0].c_str());
                    result.account_id = atoi(table_data[i][1].c_str());
                    result.coin_id = atoi(table_data[i][2].c_str());
                    result.quantity_request = atof(table_data[i][3].c_str());
                    result.commission_quantity = atof(table_data[i][4].c_str());
                    result.type = (type_DW_enum)(table_data[i][5] == "D" ? 0 : 1);
                    result.is_node = (atoi(table_data[i][6].c_str()) == 0 ? false : true);
                    result.status = status_enum::Active;

                    depositWithdrawals[result.depositWithdrawal_id]=result;
                }
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "table %s is empty", tableName.c_str());
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

bool DepositWithdrawal_dbt::Update(DepositWithdrawal depositWithdrawal)
{
    bool result = false;
    try
    {
        if (depositWithdrawal.reject_reason_id > 0)
            result = UpdateTable("depositWithdrawal_id=" + to_string(depositWithdrawal.depositWithdrawal_id), "status=%i, quantity_final=%.8f, time_execution=%s, reject_reason_id=%i",
                                 depositWithdrawal.status, depositWithdrawal.quantity_final, UtilityTools::Timestamp2DatetimeString(depositWithdrawal.time_execution).c_str(), depositWithdrawal.reject_reason_id);
        else
            result = UpdateTable("depositWithdrawal_id=" + to_string(depositWithdrawal.depositWithdrawal_id), "status=%i, quantity_final=%.8f, time_execution=%s",
                                 depositWithdrawal.status, depositWithdrawal.quantity_final, UtilityTools::Timestamp2DatetimeString(depositWithdrawal.time_execution).c_str());
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

bool DepositWithdrawal_dbt::Select(map<int,DepositWithdrawal> &depositWithdrawals, int depositWithdrawal_id)
{
    DepositWithdrawal result;
    bool res = false;
    try
    {
        std::vector<std::vector<std::string>> table_data;

        if (ReadTable(table_data, "depositWithdrawal_id, account_id, coin_id, quantity_request, commission_quantity, `type`, is_node", "depositWithdrawal_id=%i AND status=0", depositWithdrawal_id))
        {
            if (table_data.size() > 0)
            {

                result.depositWithdrawal_id = atoi(table_data[0][0].c_str());
                result.account_id = atoi(table_data[0][1].c_str());
                result.coin_id = atoi(table_data[0][2].c_str());
                result.quantity_request = atof(table_data[0][3].c_str());
                result.commission_quantity = atof(table_data[0][4].c_str());
                result.type = (type_DW_enum)(table_data[0][5] == "D" ? 0 : 1);
                result.is_node = (atoi(table_data[0][6].c_str()) == 0 ? false : true);
                result.status = status_enum::Active;

                depositWithdrawals[result.depositWithdrawal_id]=result;

                res = true;
            }
            else
            {
                Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", depositWithdrawal_id, tableName.c_str());
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