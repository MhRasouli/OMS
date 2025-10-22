#include "Coin.h"
#include "Coin_dbt.h"
#include "TradeRequestProcessor.h"
#include "PriceFeederManager.h"

Coin_dbt::Coin_dbt(DatabaseHandler *databaseHandler) : DatabaseTable(databaseHandler, "coin")
{
}

bool Coin_dbt::SelectAll(map<int, Coin> &coins)
{
	coins.clear();
	bool res = false;
	try
	{
		std::vector<std::vector<std::string>> table_data;

		if (ReadTable(table_data, "coin_id,nick_name, riskMargin_perc, \
		 lockMargin_perc, quantity_precision, tradeRequestGenerator_id, \
		 encouragement_perc, maxLimitFeedStop_sec, market_id, loopSleep_ms, \
		 coinMinBalance_alertMessageThreshold_id, tradeRequestRobotID_seller, \
		 tradeRequestRobotID_buyer, price_decimal, quantity_decimal, \
		 income_deposit_account_id, lastTradePriceGenerator_id, is_usdtBase",
					  "is_active=%i", 1))
		{
			if (table_data.size() > 0)
			{
				for (int i = 0; i < table_data.size(); i++)
				{
					Coin result;
					result.coin_id = atoi(table_data[i][0].c_str());
					result.name = table_data[i][1];
					result.riskMargin = atof(table_data[i][2].c_str()) / 100.0;
					result.lockMargin = atof(table_data[i][3].c_str()) / 100.0;
					result.quantity_precision = atoi(table_data[i][4].c_str());
					result.tradeRequestGenerator_id = atoi(table_data[i][5].c_str());
					result.encouragement_perc = atof(table_data[i][6].c_str()) / 100.0;
					result.maxLimitFeedStop_sec = atoi(table_data[i][7].c_str());
					result.market_id = atoi(table_data[i][8].c_str());
					result.loopSleep_ms = atoi(table_data[i][9].c_str());
					result.coinMinBalance_alertMessageThreshold_id = atoi(table_data[i][10].c_str());
					result.tradeRequestRobotID_seller = atoi(table_data[i][11].c_str());
					result.tradeRequestRobotID_buyer = atoi(table_data[i][12].c_str());
					result.price_decimal = atoi(table_data[i][13].c_str());
					result.quantity_decimal = atoi(table_data[i][14].c_str());
					result.income_deposit_account_id = atoi(table_data[i][15].c_str());
					result.lastTradePriceGenerator_id = atoi(table_data[i][16].c_str());
					result.is_usdtBase = atoi(table_data[i][17].c_str()) == 0 ? false : true;

					result.market_buy_price = 0;
					result.market_sell_price = 0;

					result.market_price_coefficient = result.name == "IRT" ? 0.1 : 1;

					if (coins.find(result.coin_id) == coins.end())
					{
						coins[result.coin_id] = result;

						coins[result.coin_id].tradeRequestProcessor = new TradeRequestProcessor(&coins[result.coin_id]);
						GlobalVariables::priceFeederManager.InitCoinWebsocketBox(&coins[result.coin_id]);
						Database_manager::wallet_dbt.SelectAll(coins[result.coin_id]);
						coins[result.coin_id].tradeRequestProcessor->Start(UtilityTools::StringFormat("Coin '%s'", result.name.c_str()));
						coins[result.coin_id].tradeRequestProcessor->startProccess = true;
					}
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

bool Coin_dbt::Select(map<int, Coin> &coins, int Coin_id)
{
	bool res = false;
	try
	{
		std::vector<std::vector<std::string>> table_data;

		if (ReadTable(table_data, "nick_name, riskMargin_perc, \
		 lockMargin_perc, quantity_precision, tradeRequestGenerator_id, \
		 encouragement_perc, maxLimitFeedStop_sec, market_id, loopSleep_ms, \
		 coinMinBalance_alertMessageThreshold_id, tradeRequestRobotID_seller, \
		 tradeRequestRobotID_buyer, price_decimal, quantity_decimal, \
		 income_deposit_account_id, lastTradePriceGenerator_id, is_usdtBase",
					  "coin_id=%i", Coin_id))
		{
			if (table_data.size() > 0)
			{
				Coin result;

				result.coin_id = Coin_id;
				result.name = table_data[0][0];
				result.riskMargin = atof(table_data[0][1].c_str()) / 100.0;
				result.lockMargin = atof(table_data[0][2].c_str()) / 100.0;
				result.quantity_precision = atoi(table_data[0][3].c_str());
				result.tradeRequestGenerator_id = atoi(table_data[0][4].c_str());
				result.encouragement_perc = atof(table_data[0][5].c_str()) / 100.0;
				result.maxLimitFeedStop_sec = atoi(table_data[0][6].c_str());
				result.market_id = atoi(table_data[0][7].c_str());
				result.loopSleep_ms = atoi(table_data[0][8].c_str());
				result.coinMinBalance_alertMessageThreshold_id = atoi(table_data[0][9].c_str());
				result.tradeRequestRobotID_seller = atoi(table_data[0][10].c_str());
				result.tradeRequestRobotID_buyer = atoi(table_data[0][11].c_str());
				result.price_decimal = atoi(table_data[0][12].c_str());
				result.quantity_decimal = atoi(table_data[0][13].c_str());
				result.income_deposit_account_id = atoi(table_data[0][14].c_str());
				result.lastTradePriceGenerator_id = atoi(table_data[0][15].c_str());
				result.is_usdtBase = atoi(table_data[0][16].c_str()) == 0 ? false : true;

				result.market_price_coefficient = result.name == "IRT" ? 0.1 : 1;

				if (coins.find(result.coin_id) == coins.end())
				{
					coins[result.coin_id] = result;
					coins[result.coin_id].tradeRequestProcessor = new TradeRequestProcessor(&coins[result.coin_id]);
					GlobalVariables::priceFeederManager.InitCoinWebsocketBox(&coins[result.coin_id]);
					Database_manager::wallet_dbt.SelectAll(coins[result.coin_id]);
					coins[result.coin_id].tradeRequestProcessor->Start(UtilityTools::StringFormat("Coin '%s'", result.name.c_str()));
					coins[result.coin_id].tradeRequestProcessor->startProccess = true;
				}
				else
				{
					coins[result.coin_id].name = result.name;
					coins[result.coin_id].riskMargin = result.riskMargin;
					coins[result.coin_id].lockMargin = result.lockMargin;
					coins[result.coin_id].quantity_precision = result.quantity_precision;
					coins[result.coin_id].tradeRequestGenerator_id = result.tradeRequestGenerator_id;
					coins[result.coin_id].encouragement_perc = result.encouragement_perc;
					coins[result.coin_id].maxLimitFeedStop_sec = result.maxLimitFeedStop_sec;
					coins[result.coin_id].market_id = result.market_id;
					coins[result.coin_id].loopSleep_ms = result.loopSleep_ms;
					coins[result.coin_id].coinMinBalance_alertMessageThreshold_id = result.coinMinBalance_alertMessageThreshold_id;
					coins[result.coin_id].tradeRequestRobotID_seller = result.tradeRequestRobotID_seller;
					coins[result.coin_id].tradeRequestRobotID_buyer = result.tradeRequestRobotID_buyer;
					coins[result.coin_id].price_decimal = result.price_decimal;
					coins[result.coin_id].quantity_decimal = result.quantity_decimal;
					coins[result.coin_id].income_deposit_account_id = result.income_deposit_account_id;
					coins[result.coin_id].lastTradePriceGenerator_id = result.lastTradePriceGenerator_id;
					coins[result.coin_id].is_usdtBase = result.is_usdtBase;
					coins[result.coin_id].market_price_coefficient = result.market_price_coefficient;
				}

				res = true;
			}
			else
			{
				Logger::logger.log_special(LogLocation, false, "Id %i does not exist in table %s", Coin_id, tableName.c_str());
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