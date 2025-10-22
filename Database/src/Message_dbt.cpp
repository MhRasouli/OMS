#include "Message_dbt.h"
#include "Message.h"

Message_dbt::Message_dbt(DatabaseHandler* databaseHandler) : DatabaseTable(databaseHandler, "tradingServiceMessage")
{

}

bool Message_dbt::Insert(Message& message)
{
	bool result = false;
	try 
	{
        int pk_id =0;
        if(message.coin_id>0)
        {
		    pk_id = InsertTable_IfNotExist(UtilityTools::StringFormat("(confirm is NULL OR confirm=0) AND account_id=%i AND coin_id=%i",message.account_id,message.coin_id), "message, createTime, account_id, coin_id", "'%s',%s,%i,%i",message.message.c_str(),UtilityTools::Timestamp2DatetimeString(message.createTime).c_str(),message.account_id,message.coin_id );
        }
        else
        {
		    pk_id = InsertTable_IfNotExist(UtilityTools::StringFormat("(confirm is NULL OR confirm=0) AND account_id=%i AND coin_id IS NULL",message.account_id), "message, createTime, account_id", "'%s',%s,%i",message.message.c_str(),UtilityTools::Timestamp2DatetimeString(message.createTime).c_str(),message.account_id);
        }

		if (pk_id > 0)
		{
			message.message_id = pk_id;
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