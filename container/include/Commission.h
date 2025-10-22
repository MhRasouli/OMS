#pragma once

#include "stdafx.h"
#include "Enums.h"

struct Commission
{
	int commission_id=0;
	double income_usdt= 0;
	double income_usdt_price= 0;
	double income_qty= 0;
	double income_usdt_priceDiff=0;
	double income_qtyDiff=0;

	double comm_usd_buyer=0;
	double comm_usd_seller=0;
	double comm_qty_buyer=0;
	double comm_qty_seller=0;
};