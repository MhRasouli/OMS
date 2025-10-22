#include "ChangeProcessor.h"
#include "TradeRequestProcessor.h"

ChangeProcessor::ChangeProcessor()
{
}

void ChangeProcessor::Reload_last_changes_from_database()
{
	try
	{
		if (Database_manager::change_dbt.SelectAll(GlobalVariables::changes))
		{
			std::map<int, Change>::iterator it_change = GlobalVariables::changes.begin();

			while (it_change != GlobalVariables::changes.end())
			{
				Database_manager::change_dbt.Delete(it_change->first);

				if (it_change->second.account_id != 0)
				{
					GlobalVariables::accounts[it_change->second.account_id] = Database_manager::account_dbt.Select(it_change->second.account_id);
				}
				if (it_change->second.wallet_id != 0)
				{
					Database_manager::wallet_dbt.Select(it_change->second.wallet_id);
				}
				if (it_change->second.coin_id != 0)
				{
					Database_manager::coin_dbt.Select(GlobalVariables::coins, it_change->second.coin_id);
				}
				if (it_change->second.commissionRate_id != 0)
				{
					Database_manager::commissionRate_dbt.Select(GlobalVariables::commissionRates, it_change->second.commissionRate_id);
				}
				if (it_change->second.commissionRateRule_id != 0)
				{
					Database_manager::commissionRateRule_dbt.Select(GlobalVariables::commissionRateRules, it_change->second.commissionRateRule_id);
				}
				if (it_change->second.tradeRequest_id != 0)
				{
					int tradeRequest_id = it_change->second.tradeRequest_id;
					if (it_change->second.deleted)
					{
						for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin();
							 it_coin != GlobalVariables::coins.end(); it_coin++)
						{
#define coin_tradeRequests it_coin->second.tradeRequestProcessor->tradeRequests

							if (coin_tradeRequests.find(tradeRequest_id) != coin_tradeRequests.end())
							{
								coin_tradeRequests[tradeRequest_id].mustDelete = true;

								break;
							}
						}
					}
					else
					{
						TradeRequest added_modifiedTradeRequest;
						if (Database_manager::tradeRequest_dbt.Select(tradeRequest_id, added_modifiedTradeRequest))
						{
							if (GlobalVariables::coins.find(added_modifiedTradeRequest.coin_id) != GlobalVariables::coins.end())
							{
								unique_lock lock(GlobalVariables::coins[added_modifiedTradeRequest.coin_id].tradeRequestProcessor->mux);

								if (added_modifiedTradeRequest.status != status_enum::Active) // if trade request has been canceled by user
								{
									if (GlobalVariables::coins[added_modifiedTradeRequest.coin_id].tradeRequestProcessor->tradeRequests.find(tradeRequest_id) !=
										GlobalVariables::coins[added_modifiedTradeRequest.coin_id].tradeRequestProcessor->tradeRequests.end())
									{
										GlobalVariables::coins[added_modifiedTradeRequest.coin_id].tradeRequestProcessor->tradeRequests[tradeRequest_id].mustDelete = true;
									}
									Database_manager::tradeRequest_dbt.DeactiveDependentTradeRequests(tradeRequest_id);
								}
								else
								{
									GlobalVariables::coins[added_modifiedTradeRequest.coin_id].tradeRequestProcessor->enteredTradeRequests[tradeRequest_id] = added_modifiedTradeRequest;
								}
							}
						}
					}
				}
				if (it_change->second.tradeRequestGenerator_id != 0)
				{
					Database_manager::tradeRequestGenerator_dbt.Select(GlobalVariables::tradeRequestGenerators, it_change->second.tradeRequestGenerator_id);
				}
				if (it_change->second.stairCommission_id != 0)
				{
					Database_manager::stairCommission_dbt.Select(GlobalVariables::stairCommissions, it_change->second.stairCommission_id);
				}
				if (it_change->second.depositWithdrawal_id != 0)
				{
					Database_manager::depositWithdrawal_dbt.Select(GlobalVariables::depositWithdrawals, it_change->second.depositWithdrawal_id);
				}
				if (it_change->second.coinMinBalance_alertMessageThreshold_id != 0)
				{
					Database_manager::coinMinBalance_alertMessageThreshold_dbt.Select(GlobalVariables::coinMinBalanceThresholds, it_change->second.coinMinBalance_alertMessageThreshold_id);
				}
				if (it_change->second.group_id != 0)
				{
					Database_manager::group_dbt.Select(GlobalVariables::groups, it_change->second.group_id);
				}
				if (it_change->second.buyLimitation_id != 0)
				{
					Database_manager::buyLimitation_dbt.Select(GlobalVariables::buyLimitations, it_change->second.buyLimitation_id);
				}
				if (it_change->second.groupCoinMarketCommission_id != 0)
				{
					Database_manager::groupCoinMarketCommission_dbt.Select(GlobalVariables::GroupCoinMarketCommissions, it_change->second.groupCoinMarketCommission_id);
				}
				if (it_change->second.market_id != 0)
				{
					Database_manager::market_dbt.Select(GlobalVariables::markets, it_change->second.market_id);
				}
				if (it_change->second.tradeRequestRobot_id != 0)
				{
					Database_manager::tradeRequestRobot_dbt.Select(GlobalVariables::tradeRequestRobots, it_change->second.tradeRequestRobot_id);
				}
				if (it_change->second.lastTradePriceGenerator_id != 0)
				{
					Database_manager::lastTradePriceGenerator_dbt.Select(GlobalVariables::lastTradePriceGenerators, it_change->second.lastTradePriceGenerator_id);
				}

				it_change++;
			}
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
}

void ChangeProcessor::Loop_delay()
{
	static time_t time_lastGottentime = time(0);

	do
	{
		this_thread::sleep_for(chrono::milliseconds(50));
	} while (time(0) - time_lastGottentime < 1);

	time_lastGottentime = time(0);
}

unsigned ChangeProcessor::ThreadFunction()
{
	while (isStopEventSignaled() == false)
	{
		Reload_last_changes_from_database();

		Loop_delay();
	}
	return 0;
}
