#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "Configuration.h"
#include "Database_manager.h"
#include "UtilityTools.h"
#include "CircularArray.h"
#include "Change.h"
#include "Enums.h"
#include "GlobalVariables.h"
#include "BinanceTrader.h"

class ChangeProcessor :public AThread
{
public:
	ChangeProcessor();
private:
	//const int MarketPriceIsValidUntil_sec = 5;
	//const double MarketMinimumTradePrice_usd = 10.0;

	void Reload_last_changes_from_database();
	void Load_tradeRequest_queue();
	unsigned ThreadFunction();
	void Loop_delay();
};