#include "MarketResponse_dbt.h"
#include "MarketResponse.h"

MarketResponse_dbt::MarketResponse_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "marketResponse")
{
}

bool MarketResponse_dbt::Insert(MarketResponse &marketResponse)
{
	bool result = false;
	try
	{
		int pk_id = InsertTable("symbol, orderId, orderListId, clientOrderId, transactTime, price, origQty, executedQty, cummulativeQuoteQty, status, timeInForce, `type`, side",
								"'%s','%s',%i,'%s',%s,%.8f,%.8f,%.8f,%.8f,'%s','%s','%s','%s'", marketResponse.symbol.c_str(), marketResponse.orderId.c_str(), marketResponse.orderListId, marketResponse.clientOrderId.c_str(),
								UtilityTools::Timestamp2DatetimeString(marketResponse.transactTime).c_str(), marketResponse.price, marketResponse.origQty, marketResponse.executedQty, marketResponse.cummulativeQuoteQty, marketResponse.status.c_str(),
								marketResponse.timeInForce.c_str(), marketResponse.type.c_str(), marketResponse.side.c_str());
		if (pk_id > 0)
		{
			marketResponse.marketResponse_id = pk_id;
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