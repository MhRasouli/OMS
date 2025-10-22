#include "Commission_dbt.h"
#include "Commission.h"
#include "UtilityTools.h"

Commission_dbt::Commission_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "commission")
{
}

bool Commission_dbt::Insert(Commission &commission, int coin_id, double tradePrice_buyer, double tradePrice_seller)
{
	bool result = false;
	try
	{
		double comm_qty_buyer = 0;
		double comm_qty_seller = 0;
		double comm_usd_buyer = 0;
		double comm_usd_seller = 0;

		//***************************************************************************************************
		if (UtilityTools::Coin_is_UsdtBase(coin_id))
		{
			comm_qty_buyer = commission.comm_qty_buyer;
			comm_qty_seller = commission.comm_qty_seller;
			comm_usd_buyer = commission.comm_usd_buyer;
			comm_usd_seller = commission.comm_usd_seller;
		}
		else
		{
			comm_qty_buyer = commission.comm_qty_seller; // UtilityTools::Get_quantity_DbRegion(commission.comm_qty_seller, tradePrice_seller, coin_id);
			comm_usd_buyer = commission.comm_usd_seller; // UtilityTools::Get_price_DbRegion(commission.comm_usd_seller, coin_id);

			comm_qty_seller = commission.comm_qty_buyer; // UtilityTools::Get_quantity_DbRegion(commission.comm_qty_buyer, tradePrice_buyer, coin_id);
			comm_usd_seller = commission.comm_usd_buyer; // UtilityTools::Get_price_DbRegion(commission.comm_usd_buyer, coin_id);
		}
		//***************************************************************************************************

		int pk_id = InsertTable(
			"income_usdt, income_usdt_price, income_qty, income_usdt_priceDiff, income_qtyDiff, comm_usd_buyer, comm_usd_seller, comm_qty_buyer, comm_qty_seller",
			"%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f",
			commission.income_usdt, commission.income_usdt_price, commission.income_qty, commission.income_usdt_priceDiff, commission.income_qtyDiff, comm_usd_buyer, comm_usd_seller, comm_qty_buyer, comm_qty_seller);
		if (pk_id > 0)
		{
			commission.commission_id = pk_id;
			result = true;
		}
	}
	CATCH_STATEMENT
	
	return result;
}