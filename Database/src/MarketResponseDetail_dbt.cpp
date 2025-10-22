#include "MarketResponseDetail_dbt.h"
#include "MarketResponseDetail.h"

MarketResponseDetail_dbt::MarketResponseDetail_dbt(DatabaseHandler* databaseHandler) : DatabaseTable(databaseHandler, "marketResponseDetail")
{

}

bool MarketResponseDetail_dbt::Insert(MarketResponseDetail& marketResponseDetail)
{
	bool result = false;
	try
	{
		int pk_id = InsertTable("marketResponse_id, price, qty, commission, commissionAsset, tradeId ",
			"%i,%.8f,%.8f,%.8f,'%s',%i", marketResponseDetail.marketResponse_id, marketResponseDetail.price, marketResponseDetail.qty,
			marketResponseDetail.commission, marketResponseDetail.commissionAsset.c_str(), marketResponseDetail.tradeId);
		if (pk_id > 0)
		{
			marketResponseDetail.marketResponseDetail_id = pk_id;
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