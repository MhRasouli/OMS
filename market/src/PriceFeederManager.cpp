#include "PriceFeederManager.h"
#include "GlobalVariables.h"
#include "Logger.h"

PriceFeederManager::PriceFeederManager()
{
}

void PriceFeederManager::InitCoinWebsocketBox(Coin *coin)
{
	Logger::logger.log_special(LogLocation, false, "InitCoinWebsocketBox called for coin: %s (market_id: %i)", coin->name.c_str(), coin->market_id);

	if (GlobalVariables::markets[coin->market_id].name == "Binance")
	{
		Logger::logger.log_special(LogLocation, false, "Creating BinanceFeeder for %s", coin->name.c_str());
		coin->priceFeeder = new BinanceFeeder(coin);
		coin->priceFeeder->Start("websocketBox '" + coin->name + "'");
	}
	else if (GlobalVariables::markets[coin->market_id].name == "Nobitex")
	{
		Logger::logger.log_special(LogLocation, false, "Creating NobitexFeeder for %s", coin->name.c_str());
		coin->priceFeeder = new NobitexFeeder(coin);
		coin->priceFeeder->Start("NobitexFeeder '" + coin->name + "'");
	}
	else if (GlobalVariables::markets[coin->market_id].name == "Kucoin")
	{
		Logger::logger.log_special(LogLocation, false, "Creating KucoinFeeder for %s", coin->name.c_str());
		coin->priceFeeder = new KucoinFeeder(coin);
		coin->priceFeeder->Start("KucoinFeeder '" + coin->name + "'");

		//coin->priceFeeder2 = new KucoinFeederPython(coin);
		//coin->priceFeeder2->Start("KucoinFeederPython '" + coin->name + "'");
	}
	else
	{
		Logger::logger.log_special(LogLocation, false, "Unknown market for %s:%s", coin->name.c_str(), GlobalVariables::markets[coin->market_id].name.c_str());
		// std::cout << "Unknown market for " << coin->name << ": " << GlobalVariables::markets[coin->market_id].name << std::endl;
	}
}

unsigned PriceFeederManager::ThreadFunction()
{
	while (isStopEventSignaled() == false)
	{
		for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
		{
			if (it_coin->second.market_id > 1)
			{
				if (it_coin->second.priceFeeder == 0)
				{
					InitCoinWebsocketBox(&it_coin->second);
				}
				else if (it_coin->second.priceFeeder->mustDelete)
				{
					if (GlobalVariables::markets[it_coin->second.market_id].name == "Binance")
					{
						it_coin->second.priceFeeder->StopGetData();
						delete it_coin->second.priceFeeder;
						InitCoinWebsocketBox(&it_coin->second);
					}
					else if (GlobalVariables::markets[it_coin->second.market_id].name == "Kucoin")
					{
						Logger::logger.log_special(LogLocation, false, "Renew KucoinFeeder token for %s", it_coin->second.name.c_str());
						it_coin->second.priceFeeder->RenewToken();
						it_coin->second.priceFeeder->mustDelete = false;
					}
				}
			}
		}

		this_thread::sleep_for(chrono::milliseconds(1000));
	}

	for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
	{
		if (it_coin->second.market_id > 1)
		{
			it_coin->second.priceFeeder->StopGetData();

			if (it_coin->second.priceFeeder)
			{
				it_coin->second.priceFeeder->StopGetData();
				delete it_coin->second.priceFeeder;
			}

			if (it_coin->second.priceFeeder2)
			{
				it_coin->second.priceFeeder2->StopGetData();
				delete it_coin->second.priceFeeder2;
			}
		}
	}

	return 0;
}