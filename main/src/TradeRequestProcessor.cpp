#include "TradeRequestProcessor.h"
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

TradeRequestProcessor::TradeRequestProcessor(Coin *_coin)
{
	try
	{
		coin = _coin;
		coin->previous_prices.coin_id = coin->coin_id;

		if (coin->market_id > 1)
		{
			if (GlobalVariables::markets[coin->market_id].name == "Binance")
			{
				marketTrader = new BinanceTrader(coin);
			}
			else if (GlobalVariables::markets[coin->market_id].name == "Nobitex")
			{
				marketTrader = new NobitexTrader(coin);
			}
			else if (GlobalVariables::markets[coin->market_id].name == "Kucoin")
			{
				marketTrader = new KucoinTrader(coin);
			}
		}
	}
	CATCH_STATEMENT
}

double TradeRequestProcessor::get_minimumAllowedTradeAmount()
{
	return GlobalVariables::markets[coin->market_id].minimumTradePrice_usd;
}

bool TradeRequestProcessor::TpSlProc(Trade trade, const int tpsl_RefTrade_id)
{
	bool res = false;
	try
	{
		//  Add Buyer part
		if (GlobalVariables::accountTpSls.find(trade.buyer_id) != GlobalVariables::accountTpSls.end())
		{
			if (GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls.find(trade.coin_id) != GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls.end())
			{
				TradeTpSl tradeTpSl_buy;
				tradeTpSl_buy.trade_id = trade.trade_id;
				tradeTpSl_buy.remained_qty = trade.quantity_buyer;
				tradeTpSl_buy.price = trade.price_buyer;
				tradeTpSl_buy.coin_id = trade.coin_id;
				GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_buy.push_back(tradeTpSl_buy);
			}
			else
			{
				CoinTpSl coinTpSl;
				coinTpSl.coin_id = trade.coin_id;

				TradeTpSl tradeTpSl_buy;
				tradeTpSl_buy.trade_id = trade.trade_id;
				tradeTpSl_buy.remained_qty = trade.quantity_buyer;
				tradeTpSl_buy.price = trade.price_buyer;
				tradeTpSl_buy.coin_id = trade.coin_id;
				coinTpSl.tradeTpSls_buy.push_back(tradeTpSl_buy);

				GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id] = coinTpSl;
			}
		}
		else
		{
			AccountTpSl accountTpSl;
			accountTpSl.account_id = trade.buyer_id;

			CoinTpSl coinTpSl;
			coinTpSl.coin_id = trade.coin_id;

			TradeTpSl tradeTpSl_buy;
			tradeTpSl_buy.trade_id = trade.trade_id;
			tradeTpSl_buy.remained_qty = trade.quantity_buyer;
			tradeTpSl_buy.price = trade.price_buyer;
			tradeTpSl_buy.coin_id = trade.coin_id;
			coinTpSl.tradeTpSls_buy.push_back(tradeTpSl_buy);
			accountTpSl.coinTpSls[trade.coin_id] = coinTpSl;

			GlobalVariables::accountTpSls[trade.buyer_id] = accountTpSl;
		}
		//-----------------------------------------------------------------------------------------------
		// Remove sell part
		if (GlobalVariables::accountTpSls.find(trade.seller_id) != GlobalVariables::accountTpSls.end())
		{
			if (GlobalVariables::accountTpSls[trade.seller_id].coinTpSls.find(trade.coin_id) != GlobalVariables::accountTpSls[trade.seller_id].coinTpSls.end())
			{
				if (GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.size() > 0)
				{
					if (tpsl_RefTrade_id == 0) // if Trade request was not a TP/SL type
					{
						double wallet_init_qty = GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].init_qty;
						if (wallet_init_qty > trade.quantity_seller)
						{
							GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].init_qty = wallet_init_qty - trade.quantity_seller;
						}
						else
						{
							double trade_qty_seller = trade.quantity_seller - wallet_init_qty;
							GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].init_qty = 0;

							while (trade_qty_seller > 0 && GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.size() > 0)
							{
								TradeTpSl *tradeTpSl_buy = &GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.front();
								if (coin->wallets[trade.seller_id].quantity > 0 && (tradeTpSl_buy->remained_qty - trade_qty_seller) >= 0.00000001)
								{
									tradeTpSl_buy->remained_qty -= trade_qty_seller;
									Database_manager::trade_dbt.Update_TpSl(*tradeTpSl_buy);
									Database_manager::tradeRequest_dbt.ModifyQtyActiveTpSl(*tradeTpSl_buy);
									trade_qty_seller = 0;
								}
								else
								{
									trade_qty_seller -= tradeTpSl_buy->remained_qty;
									tradeTpSl_buy->remained_qty = 0;
									Database_manager::trade_dbt.Update_TpSl(*tradeTpSl_buy);
									Database_manager::tradeRequest_dbt.RejectInactiveTpSl(tradeTpSl_buy->trade_id, 0);
									GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.pop_front();
								}
							}
						}
					}
					else // if Trade request was a TP/SL type
					{
						for (list<TradeTpSl>::iterator it_tradeTpSl_buy = GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.begin();
							 it_tradeTpSl_buy != GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.end();
							 it_tradeTpSl_buy++)
						{
							if (it_tradeTpSl_buy->trade_id == tpsl_RefTrade_id)
							{
								it_tradeTpSl_buy->remained_qty = 0;
								Database_manager::trade_dbt.Update_TpSl(*it_tradeTpSl_buy);
								Database_manager::tradeRequest_dbt.RejectInactiveTpSl(it_tradeTpSl_buy->trade_id, trade.tradeRequestSeller_id);
								GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_buy.erase(it_tradeTpSl_buy);
								break;
							}
						}
					}
				}
			}
		}
		/*
	}
	else // Coin is not UsdtBase
	{
		// Add seller part
		if (GlobalVariables::accountTpSls.find(trade.seller_id) != GlobalVariables::accountTpSls.end())
		{
			if (GlobalVariables::accountTpSls[trade.seller_id].coinTpSls.find(trade.coin_id) != GlobalVariables::accountTpSls[trade.seller_id].coinTpSls.end())
			{
				TradeTpSl tradeTpSl_sell;
				tradeTpSl_sell.trade_id = trade.trade_id;
				tradeTpSl_sell.remained_qty = trade.quantity_seller;
				tradeTpSl_sell.price = trade.price_seller;
				tradeTpSl_sell.coin_id = trade.coin_id;
				GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id].tradeTpSls_sell.push_back(tradeTpSl_sell);
			}
			else
			{
				CoinTpSl coinTpSl;
				coinTpSl.coin_id = trade.coin_id;

				TradeTpSl tradeTpSl_sell;
				tradeTpSl_sell.trade_id = trade.trade_id;
				tradeTpSl_sell.remained_qty = trade.quantity_seller;
				tradeTpSl_sell.price = trade.price_seller;
				tradeTpSl_sell.coin_id = trade.coin_id;
				coinTpSl.tradeTpSls_sell.push_back(tradeTpSl_sell);

				GlobalVariables::accountTpSls[trade.seller_id].coinTpSls[trade.coin_id] = coinTpSl;
			}

			}
			else
			{
				AccountTpSl accountTpSl;
				accountTpSl.account_id = trade.seller_id;

				CoinTpSl coinTpSl;
				coinTpSl.coin_id = trade.coin_id;

				TradeTpSl tradeTpSl_sell;
				tradeTpSl_sell.trade_id = trade.trade_id;
				tradeTpSl_sell.remained_qty = trade.quantity_seller;
				tradeTpSl_sell.price = trade.price_seller;
				tradeTpSl_sell.coin_id = trade.coin_id;
				coinTpSl.tradeTpSls_sell.push_back(tradeTpSl_sell);
				accountTpSl.coinTpSls[trade.coin_id] = coinTpSl;

				GlobalVariables::accountTpSls[trade.seller_id] = accountTpSl;
			}
			//------------------------------------------------------------------------------------------------------------------------
			// Remove buy part
			if (GlobalVariables::accountTpSls.find(trade.buyer_id) != GlobalVariables::accountTpSls.end())
			{
				if (GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls.find(trade.coin_id) != GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls.end())
				{
					if (GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.size() > 0)
					{
						if (tpsl_RefTrade_id == 0) // if Trade request was not a TP/SL type
						{
							double wallet_init_qty = GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].init_qty;
							if (wallet_init_qty > trade.quantity_buyer)
							{
								GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].init_qty = wallet_init_qty - trade.quantity_buyer;
							}
							else
							{
								double trade_qty_buyer = trade.quantity_buyer - wallet_init_qty;
								GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].init_qty = 0;

								while (trade_qty_buyer > 0 && GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.size() > 0)
								{
									TradeTpSl *tradeTpSl_sell = &GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.front();
									if (coin->wallets[trade.buyer_id].quantity > 0 && (tradeTpSl_sell->remained_qty - trade_qty_buyer) >= 0.00000001)
									{
										tradeTpSl_sell->remained_qty -= trade_qty_buyer;
										Database_manager::trade_dbt.Update_TpSl(*tradeTpSl_sell);
										Database_manager::tradeRequest_dbt.ModifyQtyActiveTpSl(*tradeTpSl_sell);
										trade_qty_buyer = 0;
									}
									else
									{
										trade_qty_buyer -= tradeTpSl_sell->remained_qty;
										tradeTpSl_sell->remained_qty = 0;
										Database_manager::trade_dbt.Update_TpSl(*tradeTpSl_sell);
										Database_manager::tradeRequest_dbt.RejectInactiveTpSl(tradeTpSl_sell->trade_id, 0);
										GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.pop_front();
									}
								}
							}
						}
						else // if Trade request was a TP/SL type
						{
							for (list<TradeTpSl>::iterator it_tradeTpSl_sell = GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.begin();
								 it_tradeTpSl_sell != GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.end();
								 it_tradeTpSl_sell++)
							{
								if (it_tradeTpSl_sell->trade_id == tpsl_RefTrade_id)
								{
									it_tradeTpSl_sell->remained_qty = 0;
									Database_manager::trade_dbt.Update_TpSl(*it_tradeTpSl_sell);
									Database_manager::tradeRequest_dbt.RejectInactiveTpSl(it_tradeTpSl_sell->trade_id, trade.tradeRequestBuyer_id);
									GlobalVariables::accountTpSls[trade.buyer_id].coinTpSls[trade.coin_id].tradeTpSls_sell.erase(it_tradeTpSl_sell);
									break;
								}
							}
						}
					}
				}
			}
		}
		*/
	}
	CATCH_STATEMENT

	return res;
}

double TradeRequestProcessor::calc_buyer_price_trade(double price_request, double coin_price, CommissionRate CR)
{
	double price_trade = 0;
	try
	{
		if (price_request == 0)
		{
			price_trade = coin_price * (1 + CR.buyer_price_perc);
		}
		else
		{
			price_trade = price_request;
		}
	}
	CATCH_STATEMENT

	return price_trade;
}

double TradeRequestProcessor::calc_buyer_price_execution(double price_request, double coin_price, CommissionRate CR)
{
	double price_execution = 0;
	try
	{
		if (price_request == 0)
		{
			price_execution = coin_price;
		}
		else
		{
			price_execution = price_request * (1 - CR.buyer_price_perc);
		}
	}
	CATCH_STATEMENT

	return price_execution;
}

double TradeRequestProcessor::calc_buyer_trade_quantity(double tradeQuantity, double totalTradePrice, double price_trade, double coin_price, CommissionRate CR, double account_balance_usdt)
{
	double trade_quantity = tradeQuantity;
	try
	{
		if (totalTradePrice > 0)
			trade_quantity = totalTradePrice / (price_trade + coin_price * CR.buyer_commission_perc);
		else if (trade_quantity * (price_trade + coin_price * CR.buyer_commission_perc) > account_balance_usdt)
			trade_quantity = account_balance_usdt / (price_trade + coin_price * CR.buyer_commission_perc);

		trade_quantity = UtilityTools::Floor(trade_quantity, coin->quantity_precision);
	}
	CATCH_STATEMENT

	return trade_quantity;
}

double TradeRequestProcessor::calc_buyer_commission_qty(double tradeQuantity, double coin_price, CommissionRate CR)
{
	double commission_qty = 0;
	try
	{
		commission_qty = tradeQuantity * CR.buyer_quantity_perc;
		if (CR.buyer_quantity_perc > 0 && (commission_qty * coin_price < CR.buyer_min_commission_usdt))
		{
			commission_qty = CR.buyer_min_commission_usdt / coin_price;
		}
	}
	CATCH_STATEMENT

	return commission_qty;
}

double TradeRequestProcessor::calc_buyer_commission_usd(double tradeQuantity, double coin_price, CommissionRate CR)
{
	double commission_usd = 0;
	try
	{
		commission_usd = tradeQuantity * coin_price * CR.buyer_commission_perc;
		if (CR.buyer_commission_perc > 0 && (commission_usd < CR.buyer_min_commission_usdt))
		{
			commission_usd = CR.buyer_min_commission_usdt;
		}

		commission_usd = std::ceil(commission_usd * 100.0) / 100.0;
	}
	CATCH_STATEMENT

	return commission_usd;
}

bool TradeRequestProcessor::IsBrokerAccount(int account_id)
{
	bool res = false;

	if ((GlobalVariables::tradeRequestGenerators.find(coin->tradeRequestGenerator_id) != GlobalVariables::tradeRequestGenerators.end() &&
		 account_id == GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator) ||

		(GlobalVariables::markets.find(coin->market_id) != GlobalVariables::markets.end() &&
		 (account_id == GlobalVariables::markets[coin->market_id].accountID_TradeWith_LessThanMinTradePrice ||
		  account_id == GlobalVariables::markets[coin->market_id].accountID_TradeWith_TradeIsNotActive ||
		  account_id == GlobalVariables::markets[coin->market_id].accountID_MarketWalletBalance)))
	{
		res = true;
	}

	return res;
}

bool TradeRequestProcessor::CaculateTradeResult_buyer(TradeRequest *tradeRequest, TradeRequest &determinative_tradeRequest, TradeResult &tradeResult, tradeLocation_enum tradeLocation, double buyer_MarketTradePrice_usdt = 0, double buyer_swap_tradePrice = 0)
{
	bool res = false;
	try
	{
		double tradeQuantity = determinative_tradeRequest.get_quantity_OmsRegion();
		if (coin->market_id != 1 && IsBrokerAccount(tradeRequest->account_id))
		{
			tradeResult.commissionRate.buyer_price_perc = 0;
			tradeResult.commissionRate.buyer_quantity_perc = 0;
			tradeResult.commissionRate.buyer_commission_perc = 0;
			tradeResult.commissionRate.seller_price_perc = 0;
			tradeResult.commissionRate.seller_quantity_perc = 0;
			tradeResult.commissionRate.seller_commission_perc = 0;

			tradeResult.price_trade = tradeRequest->get_price_OmsRegion();
			tradeResult.price_execution = tradeRequest->get_price_OmsRegion();
			tradeResult.deposit_quantity = tradeQuantity;
			tradeResult.deposit_usdt = tradeQuantity * tradeResult.price_trade;
			tradeResult.commission_qty = 0;
			tradeResult.commission_usd = 0;
			tradeResult.commission_usd_price = 0;
		}
		else
		{
			double market_sell_price = 0;

			if (buyer_MarketTradePrice_usdt > 0)
			{
				market_sell_price = buyer_MarketTradePrice_usdt;
			}
			else
			{
				if (tradeRequest->get_price_OmsRegion() == 0 && (tradeLocation == tradeLocation_enum::Broker || tradeLocation == tradeLocation_enum::Market))
				{
					if (time(0) - tradeRequest->coin_market_time_price < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec) // our market price is update enough
					{
						market_sell_price = tradeRequest->coin_market_sell_price;
					}
					else
					{
						return res;
					}
				}
				else
				{
					market_sell_price = tradeRequest->get_price_OmsRegion();
				}
			}

			// #####################################################################################################################################################################################
			if (tradeRequest->buyLimitation_id > 0)
			{
				tradeResult.commissionRate = GlobalVariables::commissionRates[GlobalVariables::buyLimitations[tradeRequest->buyLimitation_id].commissionRate_id];
			}
			else
			{
				if (!tradeRequest->Get_CommissionRate(tradeResult.commissionRate, tradeLocation))
				{
					return res;
				}
			}
			tradeResult.price_trade = calc_buyer_price_trade(tradeRequest->get_price_OmsRegion(), market_sell_price, tradeResult.commissionRate);
			tradeResult.price_execution = calc_buyer_price_execution(tradeRequest->get_price_OmsRegion(), market_sell_price, tradeResult.commissionRate);
			tradeQuantity = calc_buyer_trade_quantity(tradeQuantity, buyer_swap_tradePrice, tradeResult.price_trade, market_sell_price, tradeResult.commissionRate, GlobalVariables::accounts[tradeRequest->account_id].balance_usdt);
			tradeResult.commission_qty = calc_buyer_commission_qty(tradeQuantity, market_sell_price, tradeResult.commissionRate);

			if (buyer_MarketTradePrice_usdt > 0)
				tradeResult.deposit_quantity = tradeQuantity;
			else
				tradeResult.deposit_quantity = tradeQuantity - tradeResult.commission_qty;

			tradeResult.commission_usd = calc_buyer_commission_usd(tradeQuantity, market_sell_price, tradeResult.commissionRate);
			tradeResult.deposit_usdt = tradeQuantity * tradeResult.price_trade + tradeResult.commission_usd;
			tradeResult.commission_usd_price = (tradeResult.price_trade - market_sell_price) * tradeQuantity;
			// #####################################################################################################################################################################################
		}

		res = true;
		determinative_tradeRequest.set_new_quantity_OmsRegion(tradeQuantity);
	}
	CATCH_STATEMENT

	return res;
}

double TradeRequestProcessor::calc_seller_trade_quantity(double tradeQuantity, int account_id)
{
	double trade_quantity = tradeQuantity;
	try
	{
		if (trade_quantity > coin->wallets[account_id].quantity)
			trade_quantity = coin->wallets[account_id].quantity;
	}
	CATCH_STATEMENT

	return trade_quantity;
}

double TradeRequestProcessor::calc_seller_price_trade(double price_request, double coin_price, CommissionRate CR)
{
	double price_trade = 0;
	try
	{
		if (price_request == 0)
		{
			price_trade = coin_price * (1 - CR.seller_price_perc);
		}
		else
		{
			price_trade = price_request;
		}
	}
	CATCH_STATEMENT

	return price_trade;
}

double TradeRequestProcessor::calc_seller_price_execution(double price_request, double coin_price, CommissionRate CR)
{
	double price_execution = 0;
	try
	{
		if (price_request == 0)
		{
			price_execution = coin_price;
		}
		else
		{
			price_execution = price_request * (1 + CR.seller_price_perc);
		}
	}
	CATCH_STATEMENT

	return price_execution;
}

double TradeRequestProcessor::calc_seller_commission_qty(double tradeQuantity, double coin_price, CommissionRate CR)
{
	double commission_qty = 0;
	try
	{
		commission_qty = tradeQuantity * CR.seller_quantity_perc;
		if (CR.seller_quantity_perc > 0 && (commission_qty * coin_price < CR.seller_min_commission_usdt))
		{
			commission_qty = CR.seller_min_commission_usdt / coin_price;
		}
	}
	CATCH_STATEMENT

	return commission_qty;
}

double TradeRequestProcessor::calc_seller_commission_usd(double tradeQuantity, double coin_price, CommissionRate CR)
{
	double commission_usd = 0;
	try
	{
		commission_usd = tradeQuantity * coin_price * CR.seller_commission_perc;
		if (CR.seller_commission_perc > 0 && (commission_usd < CR.seller_min_commission_usdt))
		{
			commission_usd = CR.seller_min_commission_usdt;
		}

		commission_usd = std::ceil(commission_usd * 100.0) / 100.0;
	}
	CATCH_STATEMENT

	return commission_usd;
}

bool TradeRequestProcessor::CaculateTradeResult_seller(TradeRequest *tradeRequest, TradeRequest &determinative_tradeRequest, TradeResult &tradeResult, tradeLocation_enum tradeLocation, double seller_MarketTradePrice_usdt = 0)
{
	bool res = false;
	try
	{
		double tradeQuantity = determinative_tradeRequest.get_quantity_OmsRegion();
		if (coin->market_id != 1 && IsBrokerAccount(tradeRequest->account_id))
		{
			tradeResult.commissionRate.buyer_price_perc = 0;
			tradeResult.commissionRate.buyer_quantity_perc = 0;
			tradeResult.commissionRate.buyer_commission_perc = 0;
			tradeResult.commissionRate.seller_price_perc = 0;
			tradeResult.commissionRate.seller_quantity_perc = 0;
			tradeResult.commissionRate.seller_commission_perc = 0;

			tradeResult.price_trade = tradeRequest->get_price_OmsRegion();
			tradeResult.price_execution = tradeRequest->get_price_OmsRegion();
			tradeResult.deposit_quantity = tradeQuantity;
			tradeResult.deposit_usdt = tradeQuantity * tradeResult.price_trade;
			tradeResult.commission_qty = 0;
			tradeResult.commission_usd = 0;
			tradeResult.commission_usd_price = 0;
		}
		else
		{
			double market_buy_price = 0;
			if (seller_MarketTradePrice_usdt > 0)
			{
				market_buy_price = seller_MarketTradePrice_usdt;
			}
			else
			{
				if (tradeRequest->get_price_OmsRegion() == 0 && (tradeLocation == tradeLocation_enum::Broker || tradeLocation == tradeLocation_enum::Market))
				{
					if (time(0) - tradeRequest->coin_market_time_price < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec) // our market price is update enough
					{
						market_buy_price = tradeRequest->coin_market_buy_price;
					}
					else
					{
						return res;
					}
				}
				else
				{
					market_buy_price = tradeRequest->get_price_OmsRegion();
				}
			}
			// #####################################################################################################################################################################################
			if (tradeRequest->buyLimitation_id > 0)
			{
				tradeResult.commissionRate = GlobalVariables::commissionRates[GlobalVariables::buyLimitations[tradeRequest->buyLimitation_id].commissionRate_id];
			}
			else
			{
				if (!tradeRequest->Get_CommissionRate(tradeResult.commissionRate, tradeLocation))
				{
					return res;
				}
			}
			tradeQuantity = calc_seller_trade_quantity(tradeQuantity, tradeRequest->account_id);
			tradeResult.price_trade = calc_seller_price_trade(tradeRequest->get_price_OmsRegion(), market_buy_price, tradeResult.commissionRate);
			tradeResult.price_execution = calc_seller_price_execution(tradeRequest->get_price_OmsRegion(), market_buy_price, tradeResult.commissionRate);
			tradeResult.commission_qty = calc_seller_commission_qty(tradeQuantity, market_buy_price, tradeResult.commissionRate);
			tradeResult.deposit_quantity = tradeQuantity;
			tradeResult.commission_usd = calc_seller_commission_usd(tradeQuantity, market_buy_price, tradeResult.commissionRate);
			tradeResult.deposit_usdt = tradeQuantity * tradeResult.price_trade - (tradeResult.commission_qty * market_buy_price + tradeResult.commission_usd);
			tradeResult.commission_usd_price = (market_buy_price - tradeResult.price_trade) * tradeQuantity;
			// #####################################################################################################################################################################################
		}

		res = true;
		determinative_tradeRequest.set_new_quantity_OmsRegion(tradeQuantity);
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::Check_BuyerLimitation(TradeRequest &tr_buyer, TradeRequest &tr_seller, TradeRequest &determinative_tradeRequest)
{
	bool res = true;
	int buyLimitation_id = 0;

	try
	{
		double tradeQuantity = determinative_tradeRequest.get_quantity_OmsRegion();
		for (map<int, BuyLimitation>::iterator it_buyLimit = GlobalVariables::buyLimitations.begin();
			 it_buyLimit != GlobalVariables::buyLimitations.end(); it_buyLimit++)
		{
			if (it_buyLimit->second.enable)
			{
				if (it_buyLimit->second.coin_id == tr_seller.coin_id)
				{
					if (it_buyLimit->second.seller_id == tr_seller.account_id)
					{
						if (it_buyLimit->second.max_sell_price >= tr_seller.get_price_OmsRegion())
						{
							if (tr_buyer.map_buyerLimitation_NotAccepted.find(tr_seller.tradeRequest_id) != tr_buyer.map_buyerLimitation_NotAccepted.end() &&
								tr_buyer.map_buyerLimitation_NotAccepted[tr_seller.tradeRequest_id] >= it_buyLimit->second.max_buy_qty)
							{
								res = false;
								break;
							}
							double buyerMaxTrade_qty = Database_manager::trade_dbt.Select_buyerMaxTrade_qty(it_buyLimit->second, tr_buyer.account_id);
							if (buyerMaxTrade_qty >= it_buyLimit->second.max_buy_qty)
							{
								tr_buyer.map_buyerLimitation_NotAccepted[tr_seller.tradeRequest_id] = it_buyLimit->second.max_buy_qty;
								res = false;
								break;
							}
							else if ((buyerMaxTrade_qty + tradeQuantity) > it_buyLimit->second.max_buy_qty)
							{
								tradeQuantity = it_buyLimit->second.max_buy_qty - buyerMaxTrade_qty;
							}
							buyLimitation_id = it_buyLimit->second.buyLimitation_id;
						}
					}
				}
			}
		}

		tr_buyer.buyLimitation_id = buyLimitation_id;
		tr_seller.buyLimitation_id = buyLimitation_id;

		determinative_tradeRequest.set_new_quantity_OmsRegion(tradeQuantity);
	}
	CATCH_STATEMENT

	return res;
}

TradeRequest TradeRequestProcessor::Get_determinative_tradeRequest(TradeRequest tradeRequest_A, TradeRequest tradeRequest_B)
{
	if (tradeRequest_A.get_price_DbRegion() != 0 && tradeRequest_B.get_price_DbRegion() == 0)
		tradeRequest_B.set_price_DbRegion(tradeRequest_A.get_price_DbRegion());
	else if (tradeRequest_A.get_price_DbRegion() == 0 && tradeRequest_B.get_price_DbRegion() != 0)
		tradeRequest_A.set_price_DbRegion(tradeRequest_B.get_price_DbRegion());

	TradeResult tradeResult;
	if (tradeRequest_A.get_side_OmsRegion() == side_enum::Buy)
	{
		CaculateTradeResult_buyer(&tradeRequest_A, tradeRequest_A, tradeResult, tradeLocation_enum::Local);
	}
	else
	{
		CaculateTradeResult_seller(&tradeRequest_A, tradeRequest_A, tradeResult, tradeLocation_enum::Local);
	}

	if (tradeRequest_B.get_side_OmsRegion() == side_enum::Buy)
	{
		CaculateTradeResult_buyer(&tradeRequest_B, tradeRequest_B, tradeResult, tradeLocation_enum::Local);
	}
	else
	{
		CaculateTradeResult_seller(&tradeRequest_B, tradeRequest_B, tradeResult, tradeLocation_enum::Local);
	}
	//*************************************************************************

	// if (tradeRequest_A.get_quantity_OmsRegion() < tradeRequest_B.get_quantity_OmsRegion())
	if (tradeRequest_A.get_quantity_DbRegion() < tradeRequest_B.get_quantity_DbRegion())
	{
		return tradeRequest_A;
	}
	else
	{
		return tradeRequest_B;
	}
}

int TradeRequestProcessor::Trade_local(TradeRequest *tr_buyer, TradeRequest *tr_seller, TradeRequest &determinative_tradeRequest, tradeLocation_enum tradeLocation)
{
	int res = 0;
	try
	{
		if (tr_seller->account_id == tr_buyer->account_id)
			return res;

		TradeResult trdRes_buyer;
		TradeResult trdRes_seller;
		//**************************************************************************************************************************************
		if (tr_buyer->type == type_enum::Swap) // It is a swap order
		{
			// Refer to function TradeRequest_dbt::ActiveDependentTradeRequests descriptions
			if (tr_buyer->dependTo_TR_id < 0)
			{
				if (!CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation, 0, tr_buyer->if_buy_lostUsdt))
					return res;
			}
			else
			{
				return res;
			}
		}
		//**************************************************************************************************************************************
		TradeRequest tr_buyer_temp = *tr_buyer;
		TradeRequest tr_seller_temp = *tr_seller;

		if (tr_buyer_temp.get_price_DbRegion() == 0)
			tr_buyer_temp.set_price_DbRegion(determinative_tradeRequest.get_price_DbRegion());

		if (tr_seller_temp.get_price_DbRegion() == 0)
			tr_seller_temp.set_price_DbRegion(determinative_tradeRequest.get_price_DbRegion());

		tr_buyer_temp.set_quantity_DbRegion(determinative_tradeRequest.get_quantity_DbRegion());
		tr_seller_temp.set_quantity_DbRegion(determinative_tradeRequest.get_quantity_DbRegion());

		if (!CaculateTradeResult_seller(&tr_seller_temp, tr_seller_temp, trdRes_seller, tradeLocation))
			return res;
		if (!CaculateTradeResult_buyer(&tr_buyer_temp, tr_buyer_temp, trdRes_buyer, tradeLocation))
			return res;

		//**************************************************************************************************************************************

		Commission commission;
		commission.income_qty = trdRes_buyer.commission_qty + trdRes_seller.commission_qty;
		commission.income_usdt = trdRes_buyer.commission_usd + trdRes_seller.commission_usd;
		commission.income_usdt_price = trdRes_buyer.commission_usd_price + trdRes_seller.commission_usd_price;
		commission.income_usdt_priceDiff = (trdRes_buyer.deposit_usdt - trdRes_seller.deposit_usdt) - commission.income_usdt;
		commission.income_qtyDiff = (trdRes_seller.deposit_quantity - trdRes_buyer.deposit_quantity) - commission.income_qty;

		commission.comm_usd_buyer = trdRes_buyer.commission_usd;
		commission.comm_qty_buyer = trdRes_buyer.commission_qty;
		commission.comm_usd_seller = trdRes_seller.commission_usd;
		commission.comm_qty_seller = trdRes_seller.commission_qty;

		if (!Database_manager::commission_dbt.Insert(commission, coin->coin_id, trdRes_buyer.price_trade, trdRes_seller.price_trade))
			return res;

		Trade trade;
		trade.seller_id = tr_seller->account_id;
		trade.buyer_id = tr_buyer->account_id;
		trade.coin_id = coin->coin_id;
		trade.commission_id = commission.commission_id;
		trade.tradeRequestSeller_id = tr_seller->tradeRequest_id;
		trade.tradeRequestBuyer_id = tr_buyer->tradeRequest_id;
		trade.marketResponse_id = 0;
		trade.quantity_buyer = tr_buyer_temp.get_quantity_OmsRegion();
		trade.quantity_seller = tr_seller_temp.get_quantity_OmsRegion();
		trade.tpSl_buyer_maxQty = !IsBrokerAccount(tr_buyer->account_id) ? trdRes_buyer.deposit_quantity : 0;
		trade.tpSl_seller_maxQty = !IsBrokerAccount(tr_seller->account_id) ? trdRes_seller.deposit_quantity : 0;
		trade.price_buyer = trdRes_buyer.price_trade;
		trade.price_seller = trdRes_seller.price_trade;
		trade.time = time(0);
		trade.buyLimitation_id = tr_buyer->buyLimitation_id;
		if (!Database_manager::trade_dbt.Insert(trade))
			return res;

		//*******************************************************************************************************************
		WalletHistory walletHistory_buyer;
		walletHistory_buyer.account_id = tr_buyer->account_id;
		walletHistory_buyer.trade_id = trade.trade_id;
		walletHistory_buyer.balanceBefore_usd = GlobalVariables::accounts[tr_buyer->account_id].balance_usdt;
		walletHistory_buyer.balanceAfter_usd = GlobalVariables::accounts[tr_buyer->account_id].balance_usdt - trdRes_buyer.deposit_usdt;
		walletHistory_buyer.coin_id = coin->coin_id;
		walletHistory_buyer.coinBefore_qty = coin->wallets[tr_buyer->account_id].quantity;
		walletHistory_buyer.coinAfter_qty = coin->wallets[tr_buyer->account_id].quantity + trdRes_buyer.deposit_quantity;
		walletHistory_buyer.time = trade.time;
		if (!Database_manager::walletHistory_dbt.Insert(walletHistory_buyer, UtilityTools::Set_wallet_quantity(tr_buyer->account_id, coin->coin_id, walletHistory_buyer.coinAfter_qty)))
			return res;
		//*******************************************************************************************************************
		GlobalVariables::accounts[tr_buyer->account_id].balance_usdt = walletHistory_buyer.balanceAfter_usd;
		coin->wallets[tr_buyer->account_id].quantity = walletHistory_buyer.coinAfter_qty;
		tr_buyer->if_buy_lostUsdt = trdRes_buyer.deposit_usdt;
		tr_buyer->if_buy_obtainedCoin = trdRes_buyer.deposit_quantity;
		//*******************************************************************************************************************

		WalletHistory walletHistory_seller;
		walletHistory_seller.account_id = tr_seller->account_id;
		walletHistory_seller.trade_id = trade.trade_id;
		walletHistory_seller.balanceBefore_usd = GlobalVariables::accounts[tr_seller->account_id].balance_usdt;
		walletHistory_seller.balanceAfter_usd = GlobalVariables::accounts[tr_seller->account_id].balance_usdt + trdRes_seller.deposit_usdt;
		walletHistory_seller.coin_id = coin->coin_id;
		walletHistory_seller.coinBefore_qty = coin->wallets[tr_seller->account_id].quantity;
		walletHistory_seller.coinAfter_qty = coin->wallets[tr_seller->account_id].quantity - trdRes_seller.deposit_quantity;
		walletHistory_seller.time = trade.time;
		if (!Database_manager::walletHistory_dbt.Insert(walletHistory_seller, UtilityTools::Set_wallet_quantity(tr_seller->account_id, coin->coin_id, walletHistory_seller.coinAfter_qty)))
			return res;
		//*******************************************************************************************************************
		GlobalVariables::accounts[tr_seller->account_id].balance_usdt = walletHistory_seller.balanceAfter_usd;
		coin->wallets[tr_seller->account_id].quantity = walletHistory_seller.coinAfter_qty;
		tr_seller->if_sell_lostCoin = trdRes_seller.deposit_quantity;
		tr_seller->if_sell_obtainedUsdt = trdRes_seller.deposit_usdt;
		//*******************************************************************************************************************
		if (coin->income_deposit_account_id > 0)
		{
			WalletHistory walletHistory_commission;
			walletHistory_commission.account_id = coin->income_deposit_account_id;
			walletHistory_commission.trade_id = trade.trade_id;
			walletHistory_commission.balanceBefore_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt;
			walletHistory_commission.balanceAfter_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt + commission.income_usdt + commission.income_usdt_price;
			walletHistory_commission.coin_id = coin->coin_id;
			walletHistory_commission.coinBefore_qty = coin->wallets[coin->income_deposit_account_id].quantity;
			walletHistory_commission.coinAfter_qty = coin->wallets[coin->income_deposit_account_id].quantity + commission.income_qty;
			walletHistory_commission.time = trade.time;
			if (!Database_manager::walletHistory_dbt.Insert(walletHistory_commission, UtilityTools::Set_wallet_quantity(coin->income_deposit_account_id, coin->coin_id, walletHistory_commission.coinAfter_qty)))
				return res;
			GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt = walletHistory_commission.balanceAfter_usd;
		}
		//*******************************************************************************************************************

		if (coin->market_id == 1)
		{
			coin->market_buy_price = trade.price_seller;
			coin->market_sell_price = trade.price_buyer;
			coin->time_price = time(0);
		}

		TpSlProc(trade, tr_seller->tpsl_RefTrade_id);

		res = trade.trade_id;
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::Check_IsTradingInMarketEnable(TradeRequest &tradeRequest)
{
	bool res = false;
	try
	{
		res = tradeRequest.Check_hasCommissionRule(tradeLocation_enum::Market) &&
			  (GlobalVariables::markets[coin->market_id].tradeIsActive /*|| tradeRequest.account_id == 1005 */); // the account 1005 is robot
	}
	CATCH_STATEMENT

	return res;
}

int TradeRequestProcessor::Trade_market_sell(TradeRequest *tr_seller)
{
	int res = 0;
	try
	{
		if (tr_seller->get_side_OmsRegion() == side_enum::Sell)
		{
			function func_tradeInDemoMode = [&]() -> int
			{
				int result = 0;
				if (tr_seller->Check_hasCommissionRule(tradeLocation_enum::Broker))
				{
					TradeRequest tr_buyer_marketAgent = *tr_seller;
					tr_buyer_marketAgent.reference_tradeRequest_id = 0;
					tr_buyer_marketAgent.account_id = GlobalVariables::markets[coin->market_id].accountID_TradeWith_TradeIsNotActive;
					tr_buyer_marketAgent.set_side_DbRegion(side_enum::Buy);
					tr_buyer_marketAgent.visible = false;
					tr_buyer_marketAgent.type = type_enum::Limit;
					tr_buyer_marketAgent.isLock = false;
					tr_buyer_marketAgent.status = status_enum::Confirm;
					tr_buyer_marketAgent.set_new_price_OmsRegion(tr_seller->coin_market_buy_price);
					if (!Database_manager::tradeRequest_dbt.Insert(tr_buyer_marketAgent, tradeRequests))
						return res;

					if (Trade_local(&tr_buyer_marketAgent, tr_seller, *tr_seller, tradeLocation_enum::Broker))
					{
						result = true;
						tr_seller->status = status_enum::Confirm;
					}
					else
					{
						tr_seller->rejectReason = "local trade has been failed";
						result = false;
						tr_seller->status = status_enum::Rejected;

						tr_buyer_marketAgent.rejectReason = "local trade has been failed";
						tr_buyer_marketAgent.status = status_enum::Rejected;
						Database_manager::tradeRequest_dbt.Update(tr_buyer_marketAgent, tradeRequests);
					}
				}
				else
				{
					result = false;
				}
				tr_seller->isLock = false;
				Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);

				return result;
			};

			if (Check_IsTradingInMarketEnable(*tr_seller)) // live mode
			{
				TradeResult trdRes_seller;
				if (CaculateTradeResult_seller(tr_seller, *tr_seller, trdRes_seller, tradeLocation_enum::Market))
				{
					if (coin->wallets[GlobalVariables::markets[coin->market_id].accountID_MarketWalletBalance].quantity < trdRes_seller.deposit_quantity)
					{
						res = func_tradeInDemoMode();
						if (res > 0)
						{
							Message message;
							message.account_id = tr_seller->account_id;
							message.createTime = time(0);
							message.message = UtilityTools::StringFormat(
								"Trade No. %i was not sent to the market due to lack of sufficient %s in the treasury and the trade was done internally with account No. %i",
								res,
								coin->name.c_str(),
								GlobalVariables::markets[coin->market_id].accountID_TradeWith_TradeIsNotActive);

							Database_manager::message_dbt.Insert(message);
						}
					}
					else
					{
						MarketResponse marketResponse;
						double qty_res_market = 0;
						double price_res_market = 0;
						if (marketTrader->MarketOrder(tr_seller->tradeRequest_id, tr_seller->get_side_OmsRegion(), tr_seller->get_quantity_OmsRegion(), trdRes_seller.commission_qty, tr_seller->coin_market_buy_price, coin->coin_id, qty_res_market, price_res_market, marketResponse, tr_seller->rejectReason))
						{
							if (GlobalVariables::marketObjects.find(coin->market_id) != GlobalVariables::marketObjects.end())
								GlobalVariables::marketObjects[coin->market_id].marketWalletsSupervisor->Get_MarketWalletBalance();

							tr_seller->set_new_quantity_OmsRegion(qty_res_market, price_res_market);
							if (!CaculateTradeResult_seller(tr_seller, *tr_seller, trdRes_seller, tradeLocation_enum::Market, price_res_market))
								return res;

							int coin_id = tr_seller->coin_id;

							Commission commission;
							commission.income_usdt = trdRes_seller.commission_usd;
							commission.income_usdt_price = trdRes_seller.commission_usd_price;
							commission.income_qty = trdRes_seller.commission_qty;
							commission.comm_usd_seller = trdRes_seller.commission_usd;
							commission.comm_qty_seller = trdRes_seller.commission_qty;

							if (!Database_manager::commission_dbt.Insert(commission, coin->coin_id, 0, trdRes_seller.price_trade))
								return res;

							Trade trade;
							trade.seller_id = tr_seller->account_id;
							trade.buyer_id = 0;
							trade.coin_id = coin_id;
							trade.commission_id = commission.commission_id;
							trade.tradeRequestSeller_id = tr_seller->tradeRequest_id;
							trade.tradeRequestBuyer_id = 0;
							trade.marketResponse_id = marketResponse.marketResponse_id;
							trade.quantity_buyer = trdRes_seller.deposit_quantity;
							trade.quantity_seller = trdRes_seller.deposit_quantity;
							trade.tpSl_buyer_maxQty = 0;
							trade.tpSl_seller_maxQty = trdRes_seller.deposit_quantity;
							trade.price_buyer = trdRes_seller.price_trade;
							trade.price_seller = trdRes_seller.price_trade;
							trade.time = time(0);
							if (!Database_manager::trade_dbt.Insert(trade))
								return res;

							WalletHistory walletHistory_seller;
							walletHistory_seller.account_id = tr_seller->account_id;
							walletHistory_seller.trade_id = trade.trade_id;
							walletHistory_seller.balanceBefore_usd = GlobalVariables::accounts[tr_seller->account_id].balance_usdt;
							walletHistory_seller.balanceAfter_usd = GlobalVariables::accounts[tr_seller->account_id].balance_usdt + trdRes_seller.deposit_usdt;
							walletHistory_seller.coin_id = coin_id;
							walletHistory_seller.coinBefore_qty = coin->wallets[tr_seller->account_id].quantity;
							walletHistory_seller.coinAfter_qty = coin->wallets[tr_seller->account_id].quantity - trdRes_seller.deposit_quantity;
							walletHistory_seller.time = trade.time;
							if (!Database_manager::walletHistory_dbt.Insert(walletHistory_seller, UtilityTools::Set_wallet_quantity(tr_seller->account_id, coin->coin_id, walletHistory_seller.coinAfter_qty)))
								return res;

							//*******************************************************************************************************************
							GlobalVariables::accounts[tr_seller->account_id].balance_usdt = walletHistory_seller.balanceAfter_usd;
							coin->wallets[tr_seller->account_id].quantity = walletHistory_seller.coinAfter_qty;
							tr_seller->if_sell_lostCoin = trdRes_seller.deposit_quantity;
							tr_seller->if_sell_obtainedUsdt = trdRes_seller.deposit_usdt;
							//*******************************************************************************************************************
							if (coin->income_deposit_account_id > 0)
							{
								WalletHistory walletHistory_commission;
								walletHistory_commission.account_id = coin->income_deposit_account_id;
								walletHistory_commission.trade_id = trade.trade_id;
								walletHistory_commission.balanceBefore_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt;
								walletHistory_commission.balanceAfter_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt + commission.income_usdt + commission.income_usdt_price;
								walletHistory_commission.coin_id = coin->coin_id;
								walletHistory_commission.coinBefore_qty = coin->wallets[coin->income_deposit_account_id].quantity;
								walletHistory_commission.coinAfter_qty = coin->wallets[coin->income_deposit_account_id].quantity + commission.income_qty;
								walletHistory_commission.time = trade.time;
								if (!Database_manager::walletHistory_dbt.Insert(walletHistory_commission, UtilityTools::Set_wallet_quantity(coin->income_deposit_account_id, coin->coin_id, walletHistory_commission.coinAfter_qty)))
									return res;
								GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt = walletHistory_commission.balanceAfter_usd;
							}
							//*******************************************************************************************************************
							TpSlProc(trade, tr_seller->tpsl_RefTrade_id);

							res = trade.trade_id;
						}
						else
						{
							Logger::logger.log_special(LogLocation, true, "Execution of tradeRequest %i failed", tr_seller->tradeRequest_id);
						}
					}
				}
			}
			else // Demo mode
			{
				res = func_tradeInDemoMode();
			}
		}
	}
	CATCH_STATEMENT

	return res;
}

int TradeRequestProcessor::Trade_market_buy(TradeRequest *tr_buyer)
{
	int res = 0;
	try
	{
		if (tr_buyer->get_side_OmsRegion() == side_enum::Buy)
		{
			function func_tradeInDemoMode = [&]() -> int
			{
				int result = 0;
				if (tr_buyer->Check_hasCommissionRule(tradeLocation_enum::Broker))
				{
					TradeRequest tr_seller_marketAgent = *tr_buyer;
					tr_seller_marketAgent.reference_tradeRequest_id = 0;
					tr_seller_marketAgent.account_id = GlobalVariables::markets[coin->market_id].accountID_TradeWith_TradeIsNotActive;
					tr_seller_marketAgent.set_side_DbRegion(side_enum::Sell);
					tr_seller_marketAgent.visible = false;
					tr_seller_marketAgent.type = type_enum::Limit;
					tr_seller_marketAgent.isLock = false;
					tr_seller_marketAgent.status = status_enum::Confirm;
					tr_seller_marketAgent.set_new_price_OmsRegion(tr_buyer->coin_market_sell_price);
					if (!Database_manager::tradeRequest_dbt.Insert(tr_seller_marketAgent, tradeRequests))
						return res;

					if (Trade_local(tr_buyer, &tr_seller_marketAgent, *tr_buyer, tradeLocation_enum::Broker))
					{
						result = true;
						tr_buyer->status = status_enum::Confirm;
					}
					else
					{
						tr_buyer->rejectReason = "local trade has been failed";
						result = false;
						tr_buyer->status = status_enum::Rejected;

						tr_seller_marketAgent.rejectReason = "local trade has been failed";
						tr_seller_marketAgent.status = status_enum::Rejected;
						Database_manager::tradeRequest_dbt.Update(tr_seller_marketAgent, tradeRequests);
					}
				}
				else
				{
					result = false;
				}
				tr_buyer->isLock = false;
				Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);

				return result;
			};

			if (Check_IsTradingInMarketEnable(*tr_buyer)) // live mode
			{
				TradeResult trdRes_buyer;
				if (CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Market))
				{
					if (GlobalVariables::accounts[GlobalVariables::markets[coin->market_id].accountID_MarketWalletBalance].balance_usdt < trdRes_buyer.deposit_usdt)
					{
						res = func_tradeInDemoMode();
						if (res > 0)
						{
							Message message;
							message.account_id = tr_buyer->account_id;
							message.createTime = time(0);
							message.message = UtilityTools::StringFormat(
								"Trade No. %i was not sent to the market due to lack of sufficient USDT in the treasury and the trade was done internally with account No. %i",
								res,
								GlobalVariables::markets[coin->market_id].accountID_TradeWith_TradeIsNotActive);

							Database_manager::message_dbt.Insert(message);
						}
					}
					else
					{
						MarketResponse marketResponse;
						double qty_res_market = 0;
						double price_res_market = 0;
						if (marketTrader->MarketOrder(tr_buyer->tradeRequest_id, tr_buyer->get_side_OmsRegion(), tr_buyer->get_quantity_OmsRegion(), trdRes_buyer.commission_qty, tr_buyer->coin_market_sell_price, coin->coin_id, qty_res_market, price_res_market, marketResponse, tr_buyer->rejectReason))
						{
							if (GlobalVariables::marketObjects.find(coin->market_id) != GlobalVariables::marketObjects.end())
								GlobalVariables::marketObjects[coin->market_id].marketWalletsSupervisor->Get_MarketWalletBalance();

							tr_buyer->set_new_quantity_OmsRegion(qty_res_market, price_res_market);
							if (!CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Market, price_res_market))
								return res;

							Commission commission;
							commission.income_usdt = trdRes_buyer.commission_usd;
							commission.income_usdt_price = trdRes_buyer.commission_usd_price;
							commission.income_qty = trdRes_buyer.commission_qty;
							commission.comm_usd_buyer = trdRes_buyer.commission_usd;
							commission.comm_qty_buyer = trdRes_buyer.commission_qty;

							if (!Database_manager::commission_dbt.Insert(commission, coin->coin_id, trdRes_buyer.price_trade, 0))
								return res;

							Trade trade;
							trade.seller_id = 0;
							trade.buyer_id = tr_buyer->account_id;
							trade.coin_id = coin->coin_id;
							trade.commission_id = commission.commission_id;
							trade.tradeRequestSeller_id = 0;
							trade.tradeRequestBuyer_id = tr_buyer->tradeRequest_id;
							trade.marketResponse_id = marketResponse.marketResponse_id;
							trade.quantity_buyer = trdRes_buyer.deposit_quantity;
							trade.quantity_seller = trdRes_buyer.deposit_quantity;
							trade.tpSl_buyer_maxQty = trdRes_buyer.deposit_quantity;
							trade.tpSl_seller_maxQty = 0;
							trade.price_buyer = trdRes_buyer.price_trade;
							trade.price_seller = trdRes_buyer.price_trade;
							trade.time = time(0);
							if (!Database_manager::trade_dbt.Insert(trade))
								return res;

							WalletHistory walletHistory_buyer;
							walletHistory_buyer.account_id = tr_buyer->account_id;
							walletHistory_buyer.trade_id = trade.trade_id;
							walletHistory_buyer.balanceBefore_usd = GlobalVariables::accounts[tr_buyer->account_id].balance_usdt;
							walletHistory_buyer.balanceAfter_usd = GlobalVariables::accounts[tr_buyer->account_id].balance_usdt - trdRes_buyer.deposit_usdt;
							walletHistory_buyer.coin_id = coin->coin_id;
							walletHistory_buyer.coinBefore_qty = coin->wallets[tr_buyer->account_id].quantity;
							walletHistory_buyer.coinAfter_qty = coin->wallets[tr_buyer->account_id].quantity + trdRes_buyer.deposit_quantity;
							walletHistory_buyer.time = trade.time;
							if (!Database_manager::walletHistory_dbt.Insert(walletHistory_buyer, UtilityTools::Set_wallet_quantity(tr_buyer->account_id, coin->coin_id, walletHistory_buyer.coinAfter_qty)))
								return res;
							//*******************************************************************************************************************
							GlobalVariables::accounts[tr_buyer->account_id].balance_usdt = walletHistory_buyer.balanceAfter_usd;
							coin->wallets[tr_buyer->account_id].quantity = walletHistory_buyer.coinAfter_qty;
							tr_buyer->if_buy_lostUsdt = trdRes_buyer.deposit_usdt;
							tr_buyer->if_buy_obtainedCoin = trdRes_buyer.deposit_quantity;
							//*******************************************************************************************************************
							if (coin->income_deposit_account_id > 0)
							{
								WalletHistory walletHistory_commission;
								walletHistory_commission.account_id = coin->income_deposit_account_id;
								walletHistory_commission.trade_id = trade.trade_id;
								walletHistory_commission.balanceBefore_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt;
								walletHistory_commission.balanceAfter_usd = GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt + commission.income_usdt + commission.income_usdt_price;
								walletHistory_commission.coin_id = coin->coin_id;
								walletHistory_commission.coinBefore_qty = coin->wallets[coin->income_deposit_account_id].quantity;
								walletHistory_commission.coinAfter_qty = coin->wallets[coin->income_deposit_account_id].quantity + commission.income_qty;
								walletHistory_commission.time = trade.time;
								if (!Database_manager::walletHistory_dbt.Insert(walletHistory_commission, UtilityTools::Set_wallet_quantity(coin->income_deposit_account_id, coin->coin_id, walletHistory_commission.coinAfter_qty)))
									return res;
								GlobalVariables::accounts[coin->income_deposit_account_id].balance_usdt = walletHistory_commission.balanceAfter_usd;
							}
							//*******************************************************************************************************************
							TpSlProc(trade, 0);

							res = trade.trade_id;
						}
						else
						{
							Logger::logger.log_special(LogLocation, true, "Execution of tradeRequest %i failed", tr_buyer->tradeRequest_id);
						}
					}
				}
			}
			else // Demo mode
			{
				res = func_tradeInDemoMode();
			}
		}
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::Trade_request_is_waste(TradeRequest &tradeRequest)
{
	bool res = false;
	try
	{
		if (!this->IsBrokerAccount(tradeRequest.account_id))
		{
			if (tradeRequest.get_side_OmsRegion() == side_enum::Buy && GlobalVariables::accounts[tradeRequest.account_id].balance_usdt <= 0.01 && tradeRequest.type != type_enum::Swap)
			{
				tradeRequest.status = status_enum::Rejected;
				tradeRequest.rejectReason = "Not enough money";
				Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
				res = true;
			}
			else if (tradeRequest.get_side_OmsRegion() == side_enum::Sell && UtilityTools::Floor(coin->wallets[tradeRequest.account_id].quantity, coin->quantity_precision) <= 0)
			{
				tradeRequest.status = status_enum::Rejected;
				tradeRequest.rejectReason = "Not enough coin";
				Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
				res = true;
			}
			//********************************************************************************************************
			else if (tradeRequest.get_price_OmsRegion() != 0 && tradeRequest.get_quantity_OmsRegion() != 0 && tradeRequest.reference_tradeRequest_id == 0 && tradeRequest.type != type_enum::Swap)
			{
				if (tradeRequest.get_price_OmsRegion() * tradeRequest.get_quantity_OmsRegion() < get_minimumAllowedTradeAmount())
				{
					tradeRequest.status = status_enum::Rejected;
					tradeRequest.rejectReason = "The order amount is less than the minimum allowed amount";
					Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
					res = true;
				}
			}
			//********************************************************************************************************
		}
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::If_possible_do_trade_local(TradeRequest &tradeRequest_A, TradeRequest &tradeRequest_B, TradeRequest &determinative_tradeRequest)
{
	bool res = false;
	try
	{
		TradeRequest *tr_buyer;
		TradeRequest *tr_seller;

		if (tradeRequest_A.get_side_OmsRegion() == side_enum::Buy)
		{
			tr_buyer = &tradeRequest_A;
			tr_seller = &tradeRequest_B;
		}
		else
		{
			tr_buyer = &tradeRequest_B;
			tr_seller = &tradeRequest_A;
		}

		if (tr_buyer->Lock_order())
			Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);

		if (tr_seller->Lock_order())
			Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);

		if (Trade_local(tr_buyer, tr_seller, determinative_tradeRequest, tradeLocation_enum::Local))
		{
			if (tr_buyer->get_quantity_DbRegion() > determinative_tradeRequest.get_quantity_DbRegion())
			{
				if (!IsRobotAccount(tr_buyer->account_id) && GlobalVariables::accounts[tr_buyer->account_id].balance_usdt > 0.01 && UtilityTools::Floor(tr_buyer->get_quantity_DbRegion() - determinative_tradeRequest.get_quantity_DbRegion(), coin->quantity_precision) > 0)
				{
					TradeRequest new_tr_buyer = *tr_buyer;
					new_tr_buyer.set_quantity_DbRegion(tr_buyer->get_quantity_DbRegion() - determinative_tradeRequest.get_quantity_DbRegion());
					new_tr_buyer.isLock = false;

					if (new_tr_buyer.type == type_enum::Swap)
						new_tr_buyer.type = type_enum::Market;

					new_tr_buyer.reference_tradeRequest_id = tr_buyer->tradeRequest_id;

					Database_manager::tradeRequest_dbt.Insert(new_tr_buyer, enteredTradeRequests);
				}
				tr_buyer->set_quantity_DbRegion(determinative_tradeRequest.get_quantity_DbRegion());
			}

			//********************************************************************
			if (tr_seller->get_quantity_DbRegion() > determinative_tradeRequest.get_quantity_DbRegion())
			{
				if (!IsRobotAccount(tr_seller->account_id) && UtilityTools::Floor(coin->wallets[tr_seller->account_id].quantity, coin->quantity_precision) > 0 && UtilityTools::Floor(tr_seller->get_quantity_DbRegion() - determinative_tradeRequest.get_quantity_DbRegion(), coin->quantity_precision) > 0)
				{
					TradeRequest new_tr_seller = *tr_seller;
					new_tr_seller.set_quantity_DbRegion(tr_seller->get_quantity_DbRegion() - determinative_tradeRequest.get_quantity_DbRegion());
					new_tr_seller.isLock = false;

					new_tr_seller.reference_tradeRequest_id = tr_seller->tradeRequest_id;

					Database_manager::tradeRequest_dbt.Insert(new_tr_seller, enteredTradeRequests);
				}
				tr_seller->set_quantity_DbRegion(determinative_tradeRequest.get_quantity_DbRegion());
			}

			//********************************************************************
			if (IsRobotAccount(tr_seller->account_id) || IsRobotAccount(tr_buyer->account_id))
			{
				SyncRobotsBalance();
			}
			//********************************************************************
			// if (tr_buyer->get_price_DbRegion() == 0)
			// 	tr_buyer->set_price_DbRegion(determinative_tradeRequest.get_price_DbRegion());

			// if (tr_seller->get_price_DbRegion() == 0)
			// 	tr_seller->set_price_DbRegion(determinative_tradeRequest.get_price_DbRegion());

			tr_buyer->status = status_enum::Confirm;
			tr_seller->status = status_enum::Confirm;

			res = true;
		}

		tr_buyer->isLock = false;
		tr_seller->isLock = false;
		Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
		Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::If_possible_do_trade_Market_buy(TradeRequest *tr_buyer)
{
	try
	{
		TradeResult trdRes_buyer;
		if (CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Market) || CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Broker))
		{
			double account_balance = GlobalVariables::accounts[tr_buyer->account_id].balance_usdt;
			double coin_marketSellPrice = tr_buyer->coin_market_sell_price;

			bool is_market_order = false;
			if (tr_buyer->type == type_enum::Market || tr_buyer->stopOrder2MarketOrder_Converted) // It is a market order
			{
				is_market_order = true;
			}
			else if (tr_buyer->type == type_enum::Swap) // It is a swap order
			{
				// Refer to function TradeRequest_dbt::ActiveDependentTradeRequests descriptions
				if (tr_buyer->dependTo_TR_id < 0)
				{
					is_market_order = true;

					if (!(CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Market, 0, tr_buyer->if_buy_lostUsdt) ||
						  CaculateTradeResult_buyer(tr_buyer, *tr_buyer, trdRes_buyer, tradeLocation_enum::Broker, 0, tr_buyer->if_buy_lostUsdt)))
						return false;
				}
				else
				{
					return false;
				}
			}
			else if (tr_buyer->type == type_enum::Limit) // It is a limit order
			{
				if (tr_buyer->account_id != GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
				{
					if (coin_marketSellPrice <= tr_buyer->get_price_OmsRegion())
					{
						if (tr_buyer->visible)
						{
							tr_buyer->visible = false;
							Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
						}
					}
					else
					{
						if (!tr_buyer->visible)
						{
							tr_buyer->visible = true;
							Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
						}
					}
				}
			}

			if (is_market_order || coin_marketSellPrice * (1 - coin->lockMargin) <= tr_buyer->get_price_OmsRegion()) // Lock margin region
			{
				if (tr_buyer->account_id == GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
				{
					if (generatedTradeRequests_buy.size() > 0 && tr_buyer->tradeRequest_id == generatedTradeRequests_buy.front())
					{
						int gtrid_buy_front = generatedTradeRequests_buy.front();
						Database_manager::tradeRequest_dbt.Delete(gtrid_buy_front, tradeRequests);
						generatedTradeRequests_buy.pop_front();

						double priceLadderDistanceSizeMin_perc = 0;
						double priceLadderDistanceSizeMax_perc = 0;

						if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc))
						{
							int gtrid_buy_back = generatedTradeRequests_buy.back();

							TradeRequest new_tr_buy;
							new_tr_buy.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
							new_tr_buy.coin_id = coin->coin_id;
							new_tr_buy.set_new_price_OmsRegion(tradeRequests[gtrid_buy_back].get_price_OmsRegion() * (1 - UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
							new_tr_buy.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / new_tr_buy.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / new_tr_buy.get_price_OmsRegion()));
							new_tr_buy.set_side_DbRegion(side_enum::Buy);
							new_tr_buy.status = status_enum::Active;
							new_tr_buy.type = type_enum::Limit;
							new_tr_buy.stopOrder2MarketOrder_Converted = false;
							new_tr_buy.coinPair = coin->name;
							new_tr_buy.visible = true;

							Database_manager::tradeRequest_dbt.Insert(new_tr_buy, tradeRequests);
							generatedTradeRequests_buy.push_back(new_tr_buy.tradeRequest_id);

							int locationInGeneratedQueue = 1;
							for (list<int>::iterator it_buy = generatedTradeRequests_buy.begin(); it_buy != generatedTradeRequests_buy.end(); it_buy++)
							{
								tradeRequests[*it_buy].locationInGeneratedQueue = locationInGeneratedQueue;
								locationInGeneratedQueue++;
							}
						}
						//**************************************************************************
						if (generatedTradeRequests_sell.size() > 0)
						{
							int gtrid_sell_back = generatedTradeRequests_sell.back();
							Database_manager::tradeRequest_dbt.Delete(gtrid_sell_back, tradeRequests);
							generatedTradeRequests_sell.pop_back();

							double priceLadderDistanceSizeMin_perc = 0;
							double priceLadderDistanceSizeMax_perc = 0;

							if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc))
							{
								int gtrid_sell_front = generatedTradeRequests_sell.front();

								TradeRequest new_tr_sell;
								new_tr_sell.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
								new_tr_sell.coin_id = coin->coin_id;
								new_tr_sell.set_new_price_OmsRegion(tradeRequests[gtrid_sell_front].get_price_OmsRegion() * (1 - UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
								new_tr_sell.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / new_tr_sell.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / new_tr_sell.get_price_OmsRegion()));
								new_tr_sell.set_side_DbRegion(side_enum::Sell);
								new_tr_sell.status = status_enum::Active;
								new_tr_sell.type = type_enum::Limit;
								new_tr_sell.stopOrder2MarketOrder_Converted = false;
								new_tr_sell.coinPair = coin->name;
								new_tr_sell.visible = true;

								Database_manager::tradeRequest_dbt.Insert(new_tr_sell, tradeRequests);
								generatedTradeRequests_sell.push_front(new_tr_sell.tradeRequest_id);

								int locationInGeneratedQueue = 1;
								for (list<int>::iterator it_sell = generatedTradeRequests_sell.begin(); it_sell != generatedTradeRequests_sell.end(); it_sell++)
								{
									tradeRequests[*it_sell].locationInGeneratedQueue = locationInGeneratedQueue;
									locationInGeneratedQueue++;
								}
							}
						}
					}
					return false;
				}

				if (tr_buyer->Lock_order())
					Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);

				if (is_market_order || coin_marketSellPrice * (1 + coin->riskMargin) <= trdRes_buyer.price_execution) // risk margin region
				{
					if (trdRes_buyer.deposit_usdt > get_minimumAllowedTradeAmount()) // Trade price is more than MarketMinimumTradePrice_usd
					{
						if (Trade_market_buy(tr_buyer))
						{
							tr_buyer->status = status_enum::Confirm;
						}
						else
						{
							if (tr_buyer->status != status_enum::Rejected)
							{
								tr_buyer->rejectReason = "market trade has been failed";
								tr_buyer->status = status_enum::Rejected;
							}
							tr_buyer->isLock = false;
							Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
							return false;
						}
					}
					else if (tr_buyer->reference_tradeRequest_id != 0) // Do trade with PCM account
					{
						TradeRequest tr_seller_pcm;
						tr_seller_pcm = *tr_buyer;
						if (tr_seller_pcm.get_price_OmsRegion() == 0)
							tr_seller_pcm.set_new_price_OmsRegion(coin_marketSellPrice);

						tr_seller_pcm.reference_tradeRequest_id = 0;
						tr_seller_pcm.account_id = GlobalVariables::markets[coin->market_id].accountID_TradeWith_LessThanMinTradePrice; // 1;
						tr_seller_pcm.set_side_DbRegion(side_enum::Sell);
						tr_seller_pcm.visible = false;
						Database_manager::tradeRequest_dbt.Insert(tr_seller_pcm, tradeRequests);

						if (Trade_local(tr_buyer, &tr_seller_pcm, tr_seller_pcm, tradeLocation_enum::Broker))
						{
							tr_buyer->status = status_enum::Confirm;
							tr_seller_pcm.status = status_enum::Confirm;

							// if (tr_buyer->get_price_OmsRegion() == 0)
							// 	tr_buyer->set_new_price_OmsRegion(tr_seller_pcm.get_price_OmsRegion());
						}
						else
						{
							tr_buyer->rejectReason = "local trade has been failed";
							tr_buyer->status = status_enum::Rejected;
							tr_seller_pcm.rejectReason = "local trade has been failed";
							tr_seller_pcm.status = status_enum::Rejected;
						}

						Database_manager::tradeRequest_dbt.Update(tr_seller_pcm, tradeRequests);
					}
					else
					{
						tr_buyer->status = status_enum::Rejected;
						tr_buyer->rejectReason = "The order amount is less than the minimum allowed amount";
					}

					tr_buyer->isLock = false;
					Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
				}
			}
			else
			{
				if (tr_buyer->isLock != false)
				{
					tr_buyer->isLock = false;
					Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
				}
			}

			return true;
		}
		else
		{
			tr_buyer->isLock = false;
			Database_manager::tradeRequest_dbt.Update(*tr_buyer, tradeRequests);
		}
	}
	CATCH_STATEMENT

	return false;
}

bool TradeRequestProcessor::If_possible_do_trade_Market_sell(TradeRequest *tr_seller)
{
	try
	{
		TradeResult trdRes_seller;
		if (CaculateTradeResult_seller(tr_seller, *tr_seller, trdRes_seller, tradeLocation_enum::Market) || CaculateTradeResult_seller(tr_seller, *tr_seller, trdRes_seller, tradeLocation_enum::Broker))
		{
			int coin_id = tr_seller->coin_id;
			double account_coin_Totalqty = coin->wallets[tr_seller->account_id].quantity;
			double coin_marketBuyPrice = tr_seller->coin_market_buy_price;

			bool is_market_order = false;
			if (tr_seller->get_price_OmsRegion() == 0 || tr_seller->type == type_enum::Market || tr_seller->stopOrder2MarketOrder_Converted) // It is a market order
			{
				is_market_order = true;
			}
			else if (tr_seller->type == type_enum::Limit) // It is a limit order
			{
				if (tr_seller->account_id != GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
				{
					if (coin_marketBuyPrice >= tr_seller->get_price_OmsRegion())
					{
						if (tr_seller->visible)
						{
							tr_seller->visible = false;
							Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
						}
					}
					else
					{
						if (!tr_seller->visible)
						{
							tr_seller->visible = true;
							Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
						}
					}
				}
			}

			if (is_market_order || coin_marketBuyPrice * (1 + coin->lockMargin) >= tr_seller->get_price_OmsRegion()) // Lock margin region
			{
				if (tr_seller->account_id == GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
				{
					if (generatedTradeRequests_sell.size() > 0 && tr_seller->tradeRequest_id == generatedTradeRequests_sell.front())
					{
						if (generatedTradeRequests_buy.size() > 0)
						{
							int gtrid_buy_back = generatedTradeRequests_buy.back();
							Database_manager::tradeRequest_dbt.Delete(gtrid_buy_back, tradeRequests);
							generatedTradeRequests_buy.pop_back();

							double priceLadderDistanceSizeMin_perc = 0;
							double priceLadderDistanceSizeMax_perc = 0;
							if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc))
							{
								int gtrid_buy_front = generatedTradeRequests_buy.front();

								TradeRequest new_tr_buy;
								new_tr_buy.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
								new_tr_buy.coin_id = coin_id;
								new_tr_buy.set_new_price_OmsRegion(tradeRequests[gtrid_buy_front].get_price_OmsRegion() * (1 + UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
								new_tr_buy.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / new_tr_buy.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / new_tr_buy.get_price_OmsRegion()));
								new_tr_buy.set_side_DbRegion(side_enum::Buy);
								new_tr_buy.status = status_enum::Active;
								new_tr_buy.type = type_enum::Limit;
								new_tr_buy.stopOrder2MarketOrder_Converted = false;
								new_tr_buy.coinPair = coin->name;
								new_tr_buy.visible = true;

								Database_manager::tradeRequest_dbt.Insert(new_tr_buy, tradeRequests);
								generatedTradeRequests_buy.push_front(new_tr_buy.tradeRequest_id);

								int locationInGeneratedQueue = 1;
								for (list<int>::iterator it_buy = generatedTradeRequests_buy.begin(); it_buy != generatedTradeRequests_buy.end(); it_buy++)
								{
									tradeRequests[*it_buy].locationInGeneratedQueue = locationInGeneratedQueue;
									locationInGeneratedQueue++;
								}
							}
						}
						//**************************************************************************
						int gtrid_sell_front = generatedTradeRequests_sell.front();
						Database_manager::tradeRequest_dbt.Delete(gtrid_sell_front, tradeRequests);
						generatedTradeRequests_sell.pop_front();

						double priceLadderDistanceSizeMin_perc = 0;
						double priceLadderDistanceSizeMax_perc = 0;
						if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc))
						{
							int gtrid_sell_back = generatedTradeRequests_sell.back();

							TradeRequest new_tr_sell;
							new_tr_sell.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
							new_tr_sell.coin_id = coin_id;
							new_tr_sell.set_new_price_OmsRegion(tradeRequests[gtrid_sell_back].get_price_OmsRegion() * (1 + UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
							new_tr_sell.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / new_tr_sell.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / new_tr_sell.get_price_OmsRegion()));
							new_tr_sell.set_side_DbRegion(side_enum::Sell);
							new_tr_sell.status = status_enum::Active;
							new_tr_sell.type = type_enum::Limit;
							new_tr_sell.stopOrder2MarketOrder_Converted = false;
							new_tr_sell.coinPair = coin->name;
							new_tr_sell.visible = true;

							Database_manager::tradeRequest_dbt.Insert(new_tr_sell, tradeRequests);
							generatedTradeRequests_sell.push_back(new_tr_sell.tradeRequest_id);

							int locationInGeneratedQueue = 1;
							for (list<int>::iterator it_sell = generatedTradeRequests_sell.begin(); it_sell != generatedTradeRequests_sell.end(); it_sell++)
							{
								tradeRequests[*it_sell].locationInGeneratedQueue = locationInGeneratedQueue;
								locationInGeneratedQueue++;
							}
						}
					}
					return false;
				}

				if (tr_seller->Lock_order())
					Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);

				if (is_market_order || coin_marketBuyPrice * (1 - coin->riskMargin) >= trdRes_seller.price_execution) // risk margin region
				{
					if (trdRes_seller.deposit_usdt > get_minimumAllowedTradeAmount()) // Trade price is more than MarketMinimumTradePrice_usd
					{
						if (Trade_market_sell(tr_seller))
						{
							tr_seller->status = status_enum::Confirm;
						}
						else
						{
							if (tr_seller->status != status_enum::Rejected)
							{
								tr_seller->rejectReason = "market trade has been failed";
								tr_seller->status = status_enum::Rejected;
							}

							tr_seller->isLock = false;
							Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
							return false;
						}
					}
					else if (tr_seller->reference_tradeRequest_id != 0) // Do trade with PCM account
					{
						TradeRequest tr_buyer_pcm;
						tr_buyer_pcm = *tr_seller;
						if (tr_buyer_pcm.get_price_OmsRegion() == 0)
							tr_buyer_pcm.set_new_price_OmsRegion(coin_marketBuyPrice);

						tr_buyer_pcm.reference_tradeRequest_id = 0;
						tr_buyer_pcm.account_id = GlobalVariables::markets[coin->market_id].accountID_TradeWith_LessThanMinTradePrice; // 1;
						tr_buyer_pcm.set_side_DbRegion(side_enum::Buy);
						tr_buyer_pcm.visible = false;
						Database_manager::tradeRequest_dbt.Insert(tr_buyer_pcm, tradeRequests);
						if (Trade_local(&tr_buyer_pcm, tr_seller, tr_buyer_pcm, tradeLocation_enum::Broker))
						{
							tr_seller->status = status_enum::Confirm;
							tr_buyer_pcm.status = status_enum::Confirm;

							// if (tr_seller->get_price_OmsRegion() == 0)
							// 	tr_seller->set_new_price_OmsRegion(tr_buyer_pcm.get_price_OmsRegion());
						}
						else
						{
							tr_seller->rejectReason = "local trade has been failed";
							tr_seller->status = status_enum::Rejected;
							tr_buyer_pcm.rejectReason = "local trade has been failed";
							tr_buyer_pcm.status = status_enum::Rejected;
						}
						Database_manager::tradeRequest_dbt.Update(tr_buyer_pcm, tradeRequests);
					}
					else
					{
						tr_seller->status = status_enum::Rejected;
						tr_seller->rejectReason = "The order amount is less than the minimum allowed amount";
					}

					tr_seller->isLock = false;
					Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
				}
			}
			else
			{
				if (tr_seller->isLock != false)
				{
					tr_seller->isLock = false;
					Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
				}
			}

			return true;
		}
		else
		{
			tr_seller->isLock = false;
			Database_manager::tradeRequest_dbt.Update(*tr_seller, tradeRequests);
		}
	}
	CATCH_STATEMENT

	return false;
}

void TradeRequestProcessor::Modify_generatedTradeRequest_queue()
{
	try
	{
		double quantityChangeProbability_perc = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].quantityChangeProbability_perc;
		int minPrice_usd = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd;
		int maxPrice_usd = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd;

		double priceLadderDistanceSizeMin_perc = 0;
		double priceLadderDistanceSizeMax_perc = 0;

		for (list<int>::iterator it_buy = generatedTradeRequests_buy.begin();
			 it_buy != generatedTradeRequests_buy.end(); it_buy++)
		{
			if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc) && quantityChangeProbability_perc > 0 && coin->market_id != 1 && (time(0) - coin->time_price) < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec)
			{
				if (quantityChangeProbability_perc >= UtilityTools::Random(0, 1))
				{
					tradeRequests[*it_buy].set_new_quantity_OmsRegion(UtilityTools::Random(minPrice_usd / tradeRequests[*it_buy].get_price_OmsRegion(), maxPrice_usd / tradeRequests[*it_buy].get_price_OmsRegion()));
					if (quantityChangeProbability_perc >= UtilityTools::Random(0, 1))
					{
						tradeRequests[*it_buy].set_new_price_OmsRegion(coin->market_buy_price * (1 - tradeRequests[*it_buy].locationInGeneratedQueue * UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
					}
					Database_manager::tradeRequest_dbt.Update(tradeRequests[*it_buy], tradeRequests);
				}
			}
		}

		for (list<int>::iterator it_sell = generatedTradeRequests_sell.begin();
			 it_sell != generatedTradeRequests_sell.end(); it_sell++)
		{
			if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc) && quantityChangeProbability_perc > 0 && coin->market_id != 1 && (time(0) - coin->time_price) < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec)
			{
				if (quantityChangeProbability_perc >= UtilityTools::Random(0, 1))
				{
					tradeRequests[*it_sell].set_new_quantity_OmsRegion(UtilityTools::Random(minPrice_usd / tradeRequests[*it_sell].get_price_OmsRegion(), maxPrice_usd / tradeRequests[*it_sell].get_price_OmsRegion()));
					if (quantityChangeProbability_perc >= UtilityTools::Random(0, 1))
					{
						tradeRequests[*it_sell].set_new_price_OmsRegion(coin->market_sell_price * (1 + tradeRequests[*it_sell].locationInGeneratedQueue * UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
					}
					Database_manager::tradeRequest_dbt.Update(tradeRequests[*it_sell], tradeRequests);
				}
			}
		}
	}
	CATCH_STATEMENT
}

bool TradeRequestProcessor::Get_tradeRequestGenerator_properties(double &priceLadderDistanceSizeMin_perc, double &priceLadderDistanceSizeMax_perc)
{
	bool res = false;
	try
	{
		bool tradeRequestGenerator_isEnable = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].enable;
		if (tradeRequestGenerator_isEnable)
		{
			priceLadderDistanceSizeMin_perc = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].priceLadderDistanceSizeMin_perc;
			priceLadderDistanceSizeMax_perc = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].priceLadderDistanceSizeMax_perc;

			if (priceLadderDistanceSizeMin_perc > 0 && priceLadderDistanceSizeMin_perc <= priceLadderDistanceSizeMax_perc)
			{
				if (coin->lockMargin > priceLadderDistanceSizeMin_perc)
				{
					priceLadderDistanceSizeMax_perc = coin->lockMargin + (priceLadderDistanceSizeMax_perc - priceLadderDistanceSizeMin_perc);
					priceLadderDistanceSizeMin_perc = coin->lockMargin;
				}
				res = true;
			}
		}
	}
	CATCH_STATEMENT

	return res;
}

void TradeRequestProcessor::Generate_tradeRequest_queue()
{
	try
	{
		if (coin->market_id != 1 && (time(0) - coin->time_price <= GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec))
		{
			double priceLadderDistanceSizeMin_perc = 0;
			double priceLadderDistanceSizeMax_perc = 0;

			if (Get_tradeRequestGenerator_properties(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc))
			{
				// Generate buy orders
				while (generatedTradeRequests_buy.size() < GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].queueLength)
				{
					int queueSize = generatedTradeRequests_buy.size();

					TradeRequest tr_new_buy;
					tr_new_buy.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
					tr_new_buy.coin_id = coin->coin_id;
					tr_new_buy.locationInGeneratedQueue = queueSize + 1;
					tr_new_buy.set_new_price_OmsRegion(coin->market_buy_price * (1 - tr_new_buy.locationInGeneratedQueue * UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
					tr_new_buy.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / tr_new_buy.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / tr_new_buy.get_price_OmsRegion()));
					tr_new_buy.set_side_DbRegion(side_enum::Buy);
					tr_new_buy.status = status_enum::Active;
					tr_new_buy.type = type_enum::Limit;
					tr_new_buy.stopOrder2MarketOrder_Converted = false;
					tr_new_buy.coinPair = coin->name;
					tr_new_buy.visible = true;

					Database_manager::tradeRequest_dbt.Insert(tr_new_buy, tradeRequests);
					generatedTradeRequests_buy.push_back(tr_new_buy.tradeRequest_id);
				}
				// Generate sell orders
				while (generatedTradeRequests_sell.size() < GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].queueLength)
				{
					int queueSize = generatedTradeRequests_sell.size();

					TradeRequest tr_new_sell;
					tr_new_sell.account_id = GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator;
					tr_new_sell.coin_id = coin->coin_id;
					tr_new_sell.locationInGeneratedQueue = queueSize + 1;
					tr_new_sell.set_new_price_OmsRegion(coin->market_sell_price * (1 + tr_new_sell.locationInGeneratedQueue * UtilityTools::Random(priceLadderDistanceSizeMin_perc, priceLadderDistanceSizeMax_perc)));
					tr_new_sell.set_new_quantity_OmsRegion(UtilityTools::Random(GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].minPrice_usd / tr_new_sell.get_price_OmsRegion(), GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].maxPrice_usd / tr_new_sell.get_price_OmsRegion()));
					tr_new_sell.set_side_DbRegion(side_enum::Sell);
					tr_new_sell.status = status_enum::Active;
					tr_new_sell.type = type_enum::Limit;
					tr_new_sell.stopOrder2MarketOrder_Converted = false;
					tr_new_sell.coinPair = coin->name;
					tr_new_sell.visible = true;

					Database_manager::tradeRequest_dbt.Insert(tr_new_sell, tradeRequests);
					generatedTradeRequests_sell.push_back(tr_new_sell.tradeRequest_id);
				}
			}
		}
		else
		{
			Logger::logger.log_special_if_verb(LogLocation, true, "last price for coin %s not received.", coin->name.c_str());
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::Printout_mainLoop_log()
{
	try
	{
		if (mainLoopTimer <= 0)
		{
			mainLoopTimer = 60; // in second
			int QueueGen_size = 0;
			int TradeRequest_size = 0;

			QueueGen_size += generatedTradeRequests_buy.size();
			QueueGen_size += generatedTradeRequests_sell.size();

			for (map<int, TradeRequest>::iterator it_TR = tradeRequests.begin();
				 it_TR != tradeRequests.end(); it_TR++)
			{
				if (!it_TR->second.mustDelete)
					TradeRequest_size++;
			}
			TradeRequest_size -= QueueGen_size;
			Logger::logger.log_special(LogLocation, false, "### coin '%s' Loop : %i #### TradeRequest_size=%i, QueueGenerator_size=%i, RobotBuyer_size=%i, RobotSeller_size=%i", coin->name.c_str(), ++mainLoopCounter, TradeRequest_size, QueueGen_size, robotBuyerTRs_buy.size(), robotSellerTRs_sell.size());

			// if (coin->tradeRequestRobotID_buyer != 0 || coin->tradeRequestRobotID_seller != 0)
			//	LogOutRobotTradeRequests();
		}
		else
		{
			mainLoopTimer--;
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::Loop_delay()
{
	try
	{
		/*
		do
		{
			this_thread::sleep_for(chrono::milliseconds(coin->loopSleep_ms / 5));
		} while ((duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - time_lastGottentime) < coin->loopSleep_ms);

		time_lastGottentime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		*/
		this_thread::sleep_for(chrono::milliseconds(coin->loopSleep_ms));
	}
	CATCH_STATEMENT
}

bool TradeRequestProcessor::Check_do_local_trade(TradeRequest &tradeRequest)
{
	bool res = false;
	try
	{
		if (tradeRequest.account_id != GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator) // just for pending orders
		{
			bool BTR_isEmpty = true;
			TradeRequest determinative_tradeRequest;
			map<int, TradeRequest>::iterator it_bestTradeRequest_temp;
			if (tradeRequest.Check_hasCommissionRule(tradeLocation_enum::Local))
			{
				for (map<int, TradeRequest>::iterator it_tradeRequest_temp = tradeRequests.begin(); it_tradeRequest_temp != tradeRequests.end(); it_tradeRequest_temp++)
				{
					if (it_tradeRequest_temp->second.tradeRequest_id == tradeRequest.tradeRequest_id)
						continue;

					if (it_tradeRequest_temp->second.mustDelete)
						continue;

					if (it_tradeRequest_temp->second.get_price_OmsRegion() == 0 && tradeRequest.get_price_OmsRegion() == 0)
						continue;

					if (it_tradeRequest_temp->second.account_id == GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
						continue;

					if (Trade_request_is_waste(it_tradeRequest_temp->second))
						continue;

					if (tradeRequest.get_side_OmsRegion() != it_tradeRequest_temp->second.get_side_OmsRegion() && tradeRequest.coin_id == it_tradeRequest_temp->second.coin_id && tradeRequest.account_id != it_tradeRequest_temp->second.account_id)
					{
						if (it_tradeRequest_temp->second.get_side_OmsRegion() == side_enum::Buy)
						{

							if (tradeRequest.get_price_OmsRegion() == 0 || it_tradeRequest_temp->second.get_price_OmsRegion() == 0 ||
								tradeRequest.get_price_OmsRegion() <= it_tradeRequest_temp->second.get_price_OmsRegion())
							{
								if (BTR_isEmpty || it_bestTradeRequest_temp->second.get_price_OmsRegion() < it_tradeRequest_temp->second.get_price_OmsRegion() ||
									(it_bestTradeRequest_temp->second.get_price_OmsRegion() == it_tradeRequest_temp->second.get_price_OmsRegion() && it_bestTradeRequest_temp->second.time > it_tradeRequest_temp->second.time))
								{
									TradeRequest determinative_TR = Get_determinative_tradeRequest(it_tradeRequest_temp->second, tradeRequest);
									if (Check_BuyerLimitation(it_tradeRequest_temp->second, tradeRequest, determinative_TR))
									{
										determinative_tradeRequest = determinative_TR;
										BTR_isEmpty = false;
										it_bestTradeRequest_temp = it_tradeRequest_temp;
									}
								}
							}
						}
						else
						{
							if (tradeRequest.get_price_OmsRegion() == 0 || it_tradeRequest_temp->second.get_price_OmsRegion() == 0 ||
								tradeRequest.get_price_OmsRegion() >= it_tradeRequest_temp->second.get_price_OmsRegion())
							{
								if (BTR_isEmpty || it_bestTradeRequest_temp->second.get_price_OmsRegion() > it_tradeRequest_temp->second.get_price_OmsRegion() ||
									(it_bestTradeRequest_temp->second.get_price_OmsRegion() == it_tradeRequest_temp->second.get_price_OmsRegion() && it_bestTradeRequest_temp->second.time > it_tradeRequest_temp->second.time))
								{
									TradeRequest determinative_TR = Get_determinative_tradeRequest(it_tradeRequest_temp->second, tradeRequest);
									if (Check_BuyerLimitation(tradeRequest, it_tradeRequest_temp->second, determinative_TR))
									{
										determinative_tradeRequest = determinative_TR;
										BTR_isEmpty = false;
										it_bestTradeRequest_temp = it_tradeRequest_temp;
									}
								}
							}
						}
					}
				}
			}
			if (!BTR_isEmpty)
			{
				res = If_possible_do_trade_local(tradeRequest, it_bestTradeRequest_temp->second, determinative_tradeRequest);
			}
			else if (tradeRequest.get_price_OmsRegion() == 0 && coin->market_id == 1)
			{
				tradeRequest.status = status_enum::Rejected;
				tradeRequest.rejectReason = "There is no pending order to trade with this market order";
				Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
				res = true;
			}
		}
	}
	CATCH_STATEMENT

	return res;
}

bool TradeRequestProcessor::Check_do_market_trade(TradeRequest &tradeRequest)
{
	bool tradeDone = false;
	try
	{
		if (tradeRequest.coin_market_sell_price > 0 && tradeRequest.coin_market_buy_price > 0 && (time(0) - tradeRequest.coin_market_time_price < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec)) // our market price is update enough
		{
			if (tradeRequest.get_side_OmsRegion() == side_enum::Buy) // process buy order
			{
				tradeDone = If_possible_do_trade_Market_buy(&tradeRequest);
			}
			else // process sell order
			{
				tradeDone = If_possible_do_trade_Market_sell(&tradeRequest);
			}
		}
		else
		{
			Logger::logger.log_special_if_verb(LogLocation, true, "last price for coin %s not received.", coin->name.c_str());
		}
	}
	CATCH_STATEMENT

	return tradeDone;
}

bool TradeRequestProcessor::Check_stop_order(TradeRequest &tradeRequest)
{
	bool res = false;
	try
	{
		if (tradeRequest.type == type_enum::Stop)
		{
			res = true;
			if (coin->market_id == 1 || (time(0) - tradeRequest.coin_market_time_price < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec)) // our market price is update enough
			{
				TradeResult trdRes;
				if (tradeRequest.get_side_OmsRegion() == side_enum::Buy)
				{
					if (!CaculateTradeResult_buyer(&tradeRequest, tradeRequest, trdRes, tradeLocation_enum::Local))
						return false;
				}
				else
				{
					if (!CaculateTradeResult_seller(&tradeRequest, tradeRequest, trdRes, tradeLocation_enum::Local))
						return false;
				}

				if (!tradeRequest.stopOrder2MarketOrder_Converted)
				{
					double market_price = tradeRequest.coin_market_buy_price;
					if (tradeRequest.get_side_OmsRegion() == side_enum::Buy)
						market_price = tradeRequest.coin_market_sell_price;
					//**************************************************************************************************************************
					if (!tradeRequest.stopOrder_isMoreThanMarketPrice && !tradeRequest.stopOrder_isLessThanMarketPrice)
					{
						if (tradeRequest.get_price_OmsRegion() >= market_price)
						{
							tradeRequest.stopOrder_isMoreThanMarketPrice = true;
						}
						else
						{
							tradeRequest.stopOrder_isLessThanMarketPrice = true;
						}
					}
					//**************************************************************************************************************************
					if (tradeRequest.stopOrder_isMoreThanMarketPrice)
					{
						if (market_price * (1 + coin->lockMargin) >= tradeRequest.get_price_OmsRegion())
						{

							if (tradeRequest.Lock_order())
								Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);

							if (market_price * (1 - coin->riskMargin) >= trdRes.price_execution)
							{
								if (coin->market_id == 1)
								{
									tradeRequest.type = type_enum::Limit;
									tradeRequest.visible = true;
									Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
								}
								else
								{
									tradeRequest.stopOrder2MarketOrder_Converted = true;
								}
								res = false;
							}
						}
						else
						{
							if (tradeRequest.isLock)
							{
								tradeRequest.isLock = false;
								Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
							}
						}
					}
					else if (tradeRequest.stopOrder_isLessThanMarketPrice)
					{
						if (market_price * (1 - coin->lockMargin) <= tradeRequest.get_price_OmsRegion())
						{

							if (tradeRequest.Lock_order())
								Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);

							if (market_price * (1 + coin->riskMargin) <= trdRes.price_execution)
							{
								if (coin->market_id == 1)
								{
									tradeRequest.type = type_enum::Limit;
									tradeRequest.visible = true;
									Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
								}
								else
								{
									tradeRequest.stopOrder2MarketOrder_Converted = true;
								}
								res = false;
							}
						}
						else
						{
							if (tradeRequest.isLock)
							{
								tradeRequest.isLock = false;
								Database_manager::tradeRequest_dbt.Update(tradeRequest, tradeRequests);
							}
						}
					}
				}
			}
		}
	}
	CATCH_STATEMENT

	return res;
}

map<int, TradeRequest>::reverse_iterator TradeRequestProcessor::Get_NextTradeRequest(map<int, TradeRequest>::reverse_iterator it_tradeRequest)
{
	map<int, TradeRequest>::reverse_iterator res;
	try
	{
		if (it_tradeRequest->second.mustDelete)
		{
			if (it_tradeRequest->second.account_id == GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator)
			{
				if (it_tradeRequest->second.get_side_OmsRegion() == side_enum::Buy)
				{
					list<int>::iterator it = find(generatedTradeRequests_buy.begin(), generatedTradeRequests_buy.end(), it_tradeRequest->second.tradeRequest_id);

					if (it != generatedTradeRequests_buy.end())
						generatedTradeRequests_buy.erase(it);
				}
				else
				{
					list<int>::iterator it = find(generatedTradeRequests_sell.begin(), generatedTradeRequests_sell.end(), it_tradeRequest->second.tradeRequest_id);

					if (it != generatedTradeRequests_sell.end())
						generatedTradeRequests_sell.erase(it);
				}
			}
			else if (IsRobotAccount(it_tradeRequest->second.account_id))
			{
				std::list<int>::iterator it;
				it = std::find(robotBuyerTRs_buy.begin(), robotBuyerTRs_buy.end(), it_tradeRequest->second.tradeRequest_id);
				if (it != robotBuyerTRs_buy.end())
				{
					// LogOutInsertDeleteRobotTradeRequest("Erase___buy_0", it_tradeRequest->second.tradeRequest_id, 0, 0, true);
					robotBuyerTRs_buy.erase(it);
				}

				it = std::find(robotSellerTRs_sell.begin(), robotSellerTRs_sell.end(), it_tradeRequest->second.tradeRequest_id);
				if (it != robotSellerTRs_sell.end())
				{
					// LogOutInsertDeleteRobotTradeRequest("Erase__sell_0", it_tradeRequest->second.tradeRequest_id, 0, 0, true);
					robotSellerTRs_sell.erase(it);
				}
			}

			res = reverse_iterator(tradeRequests.erase(--it_tradeRequest.base()));
		}
		else
		{
			res = std::next(it_tradeRequest);
		}
	}
	CATCH_STATEMENT

	return res;
}

void TradeRequestProcessor::check_tradeRequest(TradeRequest &tradeRequest)
{
	try
	{
		if (tradeRequest.status != status_enum::Active)
		{
			tradeRequest.mustDelete = true;
			return;
		}

		if (coin->market_id == 1 || (time(0) - coin->time_price < GlobalVariables::markets[coin->market_id].feedPricesIsValidUntil_sec)) // our market price is update enough
		{
			tradeRequest.coin_market_buy_price = coin->market_buy_price;
			tradeRequest.coin_market_sell_price = coin->market_sell_price;
			tradeRequest.coin_market_time_price = coin->time_price;
		}

		if (Trade_request_is_waste(tradeRequest))
			return;

		if (Check_stop_order(tradeRequest))
			return;

		if (tradeRequest.type == type_enum::Stop && !tradeRequest.stopOrder2MarketOrder_Converted)
			return;

		if (Check_do_local_trade(tradeRequest))
			return;

		if (coin->market_id == 1)
			return;

		Check_do_market_trade(tradeRequest);
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::Check_trade_requests()
{
	try
	{
		for (map<int, TradeRequest>::iterator it_tradeRequest = enteredTradeRequests.begin();
			 it_tradeRequest != enteredTradeRequests.end();
			 it_tradeRequest = enteredTradeRequests.erase(it_tradeRequest))
		{
			if (tradeRequests.find(it_tradeRequest->second.tradeRequest_id) == tradeRequests.end() || !tradeRequests[it_tradeRequest->second.tradeRequest_id].mustDelete)
			{
				tradeRequests[it_tradeRequest->second.tradeRequest_id] = it_tradeRequest->second;
				if (IsRobotAccount(tradeRequests[it_tradeRequest->second.tradeRequest_id].account_id))
				{
					if (GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].account_id == tradeRequests[it_tradeRequest->second.tradeRequest_id].account_id)
					{
						robotBuyerTRs_buy.push_back(tradeRequests[it_tradeRequest->second.tradeRequest_id].tradeRequest_id);
					}
					else
					{
						robotSellerTRs_sell.push_back(tradeRequests[it_tradeRequest->second.tradeRequest_id].tradeRequest_id);
					}
				}
				check_tradeRequest(tradeRequests[it_tradeRequest->second.tradeRequest_id]);
			}
		}

		map<int, TradeRequest>::reverse_iterator it_tradeRequest = tradeRequests.rbegin();
		while (it_tradeRequest != tradeRequests.rend())
		{
			if (!it_tradeRequest->second.mustDelete)
			{
				check_tradeRequest(it_tradeRequest->second);
			}

			it_tradeRequest = Get_NextTradeRequest(it_tradeRequest);
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::updateCoinPrices()
{
	try
	{
		if (coin->market_id == 1)
		{
			int bestTradeRequestID_buy = 0;
			int secondTradeRequestID_buy = 0;
			int bestTradeRequestID_sell = 0;
			int secondTradeRequestID_sell = 0;
			Get_bestSecondTradeRequestID(side_enum::Buy, bestTradeRequestID_buy, secondTradeRequestID_buy);
			Get_bestSecondTradeRequestID(side_enum::Sell, bestTradeRequestID_sell, secondTradeRequestID_sell);
			//***************************************************************************************************** */
			if (bestTradeRequestID_buy != 0 && bestTradeRequestID_sell != 0)
			{
				if (coin->previous_prices.bid != tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion() || coin->previous_prices.ask != tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion())
				{
					coin->previous_prices.bid = tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion();
					coin->previous_prices.bid_qty = tradeRequests[bestTradeRequestID_buy].get_quantity_OmsRegion();

					coin->previous_prices.ask = tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion();
					coin->previous_prices.ask_qty = tradeRequests[bestTradeRequestID_sell].get_quantity_OmsRegion();

					Database_manager::prices_dbt.Update_bid_ask(coin->previous_prices);
				}
			}

			if (coin->previous_prices.ltp != coin->market_sell_price)
			{
				if (coin->previous_prices.ltp < coin->market_sell_price)
					coin->previous_prices.ltp_flag = 'U';
				else if (coin->previous_prices.ltp > coin->market_sell_price)
					coin->previous_prices.ltp_flag = 'D';

				coin->previous_prices.ltp = coin->market_sell_price;

				Database_manager::prices_dbt.Update_ltp(coin->previous_prices);
			}
		}
		else
		{
			if (coin->market_buy_price != 0 && coin->market_sell_price != 0)
			{
				if (coin->previous_prices.bid != coin->market_buy_price || coin->previous_prices.ask != coin->market_sell_price)
				{
					coin->previous_prices.bid = coin->market_buy_price;
					coin->previous_prices.bid_qty = coin->market_buy_quantity;

					coin->previous_prices.ask = coin->market_sell_price;
					coin->previous_prices.ask_qty = coin->market_sell_quantity;
					//****************************************************************************************************
					if (coin->lastTradePriceGenerator_id > 0)
					{
						if (time(0) - time_lastGenerated_ltp > UtilityTools::Random(GlobalVariables::lastTradePriceGenerators[coin->lastTradePriceGenerator_id].generateDelay_min_sec,
																					GlobalVariables::lastTradePriceGenerators[coin->lastTradePriceGenerator_id].generateDelay_max_sec))
						{
							time_lastGenerated_ltp = time(0);
							double ltp = (coin->market_buy_price + coin->market_sell_price) / 2.0;

							double distance = ltp - coin->market_buy_price;
							double min_Price = ltp - distance * GlobalVariables::lastTradePriceGenerators[coin->lastTradePriceGenerator_id].priceAreaAroundAvgOfAskBid_min_perc;
							double max_Price = ltp + distance * GlobalVariables::lastTradePriceGenerators[coin->lastTradePriceGenerator_id].priceAreaAroundAvgOfAskBid_max_perc;

							ltp = UtilityTools::Floor(UtilityTools::Random(min_Price, max_Price), coin->price_decimal);

							if (coin->previous_prices.ltp < ltp)
								coin->previous_prices.ltp_flag = 'U';
							else if (coin->previous_prices.ltp > ltp)
								coin->previous_prices.ltp_flag = 'D';

							coin->previous_prices.ltp = ltp;
						}
					}
					//****************************************************************************************************

					Database_manager::prices_dbt.Update(coin->previous_prices);
				}
			}
		}
	}
	CATCH_STATEMENT
}

unsigned TradeRequestProcessor::ThreadFunction()
{
	while ((!startProccess && !isStopEventSignaled()) || (coin->market_id != 1 && coin->time_price == 0))
	{
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	Database_manager::tradeRequest_dbt.SelectAll(coin->coin_id, tradeRequests);
	Database_manager::tradeRequest_dbt.Select_generated(generatedTradeRequests_buy, GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator, coin->coin_id, side_enum::Buy, tradeRequests);
	Database_manager::tradeRequest_dbt.Select_generated(generatedTradeRequests_sell, GlobalVariables::tradeRequestGenerators[coin->tradeRequestGenerator_id].accountID_Generator, coin->coin_id, side_enum::Sell, tradeRequests);

	Initial_lstRobotSellerTRs();
	Initial_lstRobotBuyerTRs();

	while (isStopEventSignaled() == false)
	{
		try
		{
			unique_lock lock(mux);
			Generate_tradeRequest_queue();
			Printout_mainLoop_log();
			Modify_generatedTradeRequest_queue();
			Check_trade_requests();
			RobotSeller_TradeRequest_queue();
			Check_trade_requests();
			RobotBuyer_TradeRequest_queue();
			updateCoinPrices();
		}
		CATCH_STATEMENT

		Loop_delay();
	}
	return 0;
}

// Robot functions *******************************************************************************************************************
bool TradeRequestProcessor::IsRobotAccount(int account_id)
{
	bool result = false;
	try
	{
		if (coin->tradeRequestRobotID_buyer != 0 &&
			GlobalVariables::tradeRequestRobots.find(coin->tradeRequestRobotID_buyer) != GlobalVariables::tradeRequestRobots.end() &&
			GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].enable &&
			GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].account_id == account_id)
		{
			result = true;
		}
		else if (coin->tradeRequestRobotID_seller != 0 &&
				 GlobalVariables::tradeRequestRobots.find(coin->tradeRequestRobotID_seller) != GlobalVariables::tradeRequestRobots.end() &&
				 GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller].enable &&
				 GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller].account_id == account_id)
		{
			result = true;
		}
	}
	CATCH_STATEMENT

	return result;
}

void TradeRequestProcessor::Initial_lstRobotBuyerTRs()
{
	try
	{
		if (coin->tradeRequestRobotID_buyer != 0 && GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].enable)
		{
			TradeRequestRobot robotBuyer = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer];

			robotBuyerTRs_buy.clear();

			for (map<int, TradeRequest>::iterator it_tradeRequest = tradeRequests.begin();
				 it_tradeRequest != tradeRequests.end();
				 it_tradeRequest++)
			{
				if (!it_tradeRequest->second.mustDelete && it_tradeRequest->second.account_id == robotBuyer.account_id)
				{
					if (it_tradeRequest->second.get_side_OmsRegion() == side_enum::Buy)
					{
						robotBuyerTRs_buy.push_back(it_tradeRequest->second.tradeRequest_id);
					}
				}
			}
			// sort robotTRs_buy ******************************************************************************************************

			for (list<int>::iterator it1 = robotBuyerTRs_buy.begin(); it1 != robotBuyerTRs_buy.end(); it1++)
			{
				for (list<int>::iterator it2 = it1; it2 != robotBuyerTRs_buy.end(); it2++)
				{
					if (tradeRequests[*it1].get_price_OmsRegion() > tradeRequests[*it2].get_price_OmsRegion())
					{
						int temp = *it1;
						*it1 = *it2;
						*it2 = temp;
					}
				}
			}
			//******************************************************************************************************
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::Initial_lstRobotSellerTRs()
{
	try
	{
		if (coin->tradeRequestRobotID_seller != 0 && GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller].enable)
		{
			TradeRequestRobot robotSeller = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller];

			robotSellerTRs_sell.clear();

			for (map<int, TradeRequest>::iterator it_tradeRequest = tradeRequests.begin();
				 it_tradeRequest != tradeRequests.end();
				 it_tradeRequest++)
			{
				if (!it_tradeRequest->second.mustDelete && it_tradeRequest->second.account_id == robotSeller.account_id)
				{
					if (it_tradeRequest->second.get_side_OmsRegion() == side_enum::Sell)
					{
						robotSellerTRs_sell.push_back(it_tradeRequest->second.tradeRequest_id);
					}
				}
			}
			// sort robotTRs_sell ******************************************************************************************************
			for (list<int>::iterator it1 = robotSellerTRs_sell.begin(); it1 != robotSellerTRs_sell.end(); it1++)
			{
				for (list<int>::iterator it2 = it1; it2 != robotSellerTRs_sell.end(); it2++)
				{
					if (tradeRequests[*it1].get_price_OmsRegion() < tradeRequests[*it2].get_price_OmsRegion())
					{
						int temp = *it1;
						*it1 = *it2;
						*it2 = temp;
					}
				}
			}
			//******************************************************************************************************
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::LogOutInsertDeleteRobotTradeRequest(string str_type, int tradeRequest_id, int bestTradeRequestID_buy, int bestTradeRequestID_sell, bool calledBeforeFunction)
{
	try
	{
		if (calledBeforeFunction)
			Logger::logger.log_special(LogLocation, false, "Called before function---------------------------------------");
		else
			Logger::logger.log_special(LogLocation, false, "Called after function----------------------------------------");

		Logger::logger.log_special(LogLocation, false, "Robot Trade Request:%s, id:%i", str_type.c_str(), tradeRequest_id);
		Logger::logger.log_special(LogLocation, false, "bestTradeRequestID__buy:%i", bestTradeRequestID_buy);
		Logger::logger.log_special(LogLocation, false, "bestTradeRequestID_sell:%i", bestTradeRequestID_sell);

		LogOutRobotTradeRequests();
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::LogOutRobotTradeRequests()
{
	try
	{
		Logger::logger.log_special(LogLocation, false, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");

		Logger::logger.log_special(LogLocation, false, "buy__front:%i, buy__back:%i", robotBuyerTRs_buy.front(), robotBuyerTRs_buy.back());
		Logger::logger.log_special(LogLocation, false, "sell_front:%i, sell_back:%i", robotSellerTRs_sell.front(), robotSellerTRs_sell.back());
		Logger::logger.log_special(LogLocation, false, "**************");
		for (list<int>::iterator it = robotBuyerTRs_buy.begin(); it != robotBuyerTRs_buy.end(); it++)
		{
			Logger::logger.log_special(LogLocation, false, "robotTRs_buy[%p]: %i", it, *it);
		}
		Logger::logger.log_special(LogLocation, false, "**************");
		for (list<int>::iterator it = robotSellerTRs_sell.begin(); it != robotSellerTRs_sell.end(); it++)
		{
			Logger::logger.log_special(LogLocation, false, "robotTRs_sell[%p]: %i", it, *it);
		}

		Logger::logger.log_special(LogLocation, false, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}
	CATCH_STATEMENT
}

bool TradeRequestProcessor::IsSamePriceExistInVector(double price, list<int> &robotTRs)
{
	bool isExist = false;
	try
	{
		for (list<int>::iterator it = robotTRs.begin(); it != robotTRs.end(); it++)
		{
			if (tradeRequests[*it].get_price_OmsRegion() == price)
			{
				isExist = true;
				break;
			}
		}
	}
	CATCH_STATEMENT

	return isExist;
}

void TradeRequestProcessor::Generate_OneRobotTradeRequest_sell(TradeRequestRobot &robot, int trID_bestBuy, int trID_bestSell)
{
	try
	{
		if (robotActionDelay_noOrder_seller > 0)
		{
			robotActionDelay_noOrder_seller--;
		}
		else if ((tradeRequests[trID_bestSell].get_price_OmsRegion() - tradeRequests[trID_bestBuy].get_price_OmsRegion()) > robot.minPriceDifferenceOfBestBuySell_usdt)
		{
			TradeRequest tr_new_sell;
			tr_new_sell.account_id = robot.account_id;
			tr_new_sell.coin_id = coin->coin_id;
			tr_new_sell.locationInGeneratedQueue = robotSellerTRs_sell.size() + 1;

			if (IsRobotAccount(tradeRequests[trID_bestBuy].account_id) && UtilityTools::Random(0, 100) < robot.probabilityOfTradeWithOppositeRobot_perc)
			{
				tr_new_sell.set_new_price_OmsRegion(tradeRequests[trID_bestBuy].get_price_OmsRegion());
				tr_new_sell.set_new_quantity_OmsRegion(tradeRequests[trID_bestBuy].get_quantity_OmsRegion());
			}
			else
			{
				int tryCounter = 100;
				do
				{
					tr_new_sell.set_new_price_OmsRegion(UtilityTools::Floor(tradeRequests[trID_bestSell].get_price_OmsRegion() - tradeRequests[trID_bestSell].get_price_OmsRegion() * UtilityTools::Random(robot.price_distanceFrombestRequest_min_perc, robot.price_distanceFrombestRequest_max_perc) / 100.0, robot.price_precision));
				} while (--tryCounter > 0 && (tr_new_sell.get_price_OmsRegion() < (tradeRequests[trID_bestBuy].get_price_OmsRegion() + robot.minPriceDifferenceOfBestBuySell_usdt) || IsSamePriceExistInVector(tr_new_sell.get_price_OmsRegion(), robotSellerTRs_sell)));

				if (tryCounter < 1)
					return;

				tr_new_sell.set_new_quantity_OmsRegion(UtilityTools::Floor(UtilityTools::Random(robot.quantity_distanceFrombestRequest_min_usdt / tr_new_sell.get_price_OmsRegion(), robot.quantity_distanceFrombestRequest_max_usdt / tr_new_sell.get_price_OmsRegion()), robot.quantity_precision));
			}
			tr_new_sell.set_side_DbRegion(side_enum::Sell);
			tr_new_sell.status = status_enum::Active;
			tr_new_sell.type = type_enum::Limit;
			tr_new_sell.stopOrder2MarketOrder_Converted = false;
			tr_new_sell.coinPair = coin->name;
			tr_new_sell.visible = true;
			tr_new_sell.time = time(0);

			if (coin->wallets.find(robot.account_id) != coin->wallets.end() &&
				tr_new_sell.get_quantity_OmsRegion() <= coin->wallets[robot.account_id].quantity &&
				(coin->wallets[robot.account_id].quantity * tr_new_sell.get_price_OmsRegion()) > get_minimumAllowedTradeAmount())
			{
				robotActionDelay_noOrder_seller = (int)UtilityTools::Random(robot.noOrderDelay_min_sec, robot.noOrderDelay_max_sec);

				Database_manager::tradeRequest_dbt.Insert(tr_new_sell);
				// LogOutInsertDeleteRobotTradeRequest("Insert_sell_1", tr_new_sell.tradeRequest_id, trID_bestBuy, trID_bestSell, false);
			}
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::Generate_OneRobotTradeRequest_buy(TradeRequestRobot &robot, int trID_bestBuy, int trID_bestSell)
{
	try
	{
		if (robotActionDelay_noOrder_buyer > 0)
		{
			robotActionDelay_noOrder_buyer--;
		}
		else if ((tradeRequests[trID_bestSell].get_price_OmsRegion() - tradeRequests[trID_bestBuy].get_price_OmsRegion()) > robot.minPriceDifferenceOfBestBuySell_usdt)
		{
			TradeRequest tr_new_buy;
			tr_new_buy.account_id = robot.account_id;
			tr_new_buy.coin_id = coin->coin_id;
			tr_new_buy.locationInGeneratedQueue = robotBuyerTRs_buy.size() + 1;

			if (IsRobotAccount(tradeRequests[trID_bestSell].account_id) && UtilityTools::Random(0, 100) < robot.probabilityOfTradeWithOppositeRobot_perc)
			{
				tr_new_buy.set_new_price_OmsRegion(tradeRequests[trID_bestSell].get_price_OmsRegion());
				tr_new_buy.set_new_quantity_OmsRegion(tradeRequests[trID_bestSell].get_quantity_OmsRegion());
			}
			else
			{
				int tryCounter = 100;
				do
				{
					tr_new_buy.set_new_price_OmsRegion(UtilityTools::Floor(tradeRequests[trID_bestBuy].get_price_OmsRegion() + tradeRequests[trID_bestBuy].get_price_OmsRegion() * UtilityTools::Random(robot.price_distanceFrombestRequest_min_perc, robot.price_distanceFrombestRequest_max_perc) / 100.0, robot.price_precision));
				} while (--tryCounter > 0 && (tr_new_buy.get_price_OmsRegion() > (tradeRequests[trID_bestSell].get_price_OmsRegion() - robot.minPriceDifferenceOfBestBuySell_usdt) || IsSamePriceExistInVector(tr_new_buy.get_price_OmsRegion(), robotBuyerTRs_buy)));

				if (tryCounter < 1)
					return;

				tr_new_buy.set_new_quantity_OmsRegion(UtilityTools::Floor(UtilityTools::Random(robot.quantity_distanceFrombestRequest_min_usdt / tr_new_buy.get_price_OmsRegion(), robot.quantity_distanceFrombestRequest_max_usdt / tr_new_buy.get_price_OmsRegion()), robot.quantity_precision));
			}
			tr_new_buy.set_side_DbRegion(side_enum::Buy);
			tr_new_buy.status = status_enum::Active;
			tr_new_buy.type = type_enum::Limit;
			tr_new_buy.stopOrder2MarketOrder_Converted = false;
			tr_new_buy.coinPair = coin->name;
			tr_new_buy.visible = true;
			tr_new_buy.time = time(0);

			if ((tr_new_buy.get_quantity_OmsRegion() * tr_new_buy.get_price_OmsRegion()) <= GlobalVariables::accounts[robot.account_id].balance_usdt &&
				GlobalVariables::accounts[robot.account_id].balance_usdt > get_minimumAllowedTradeAmount())
			{
				robotActionDelay_noOrder_buyer = (int)UtilityTools::Random(robot.noOrderDelay_min_sec, robot.noOrderDelay_max_sec);

				Database_manager::tradeRequest_dbt.Insert(tr_new_buy);
				// LogOutInsertDeleteRobotTradeRequest("Insert__buy_1", tr_new_buy.tradeRequest_id, trID_bestBuy, trID_bestSell, false);
			}
		}
	}
	CATCH_STATEMENT
}

bool TradeRequestProcessor::SyncRobotsBalance()
{
	bool res = false;
	try
	{
		if (coin->tradeRequestRobotID_buyer != 0 && GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].enable &&
			coin->tradeRequestRobotID_seller != 0 && GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller].enable)
		{
			TradeRequestRobot robot_buyer = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer];
			TradeRequestRobot robot_seller = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller];

			//*******************************************************************************************************************
			WalletHistory walletHistory_buyer;
			walletHistory_buyer.account_id = robot_buyer.account_id;
			walletHistory_buyer.balanceBefore_usd = GlobalVariables::accounts[robot_buyer.account_id].balance_usdt;
			walletHistory_buyer.balanceAfter_usd = GlobalVariables::accounts[robot_buyer.account_id].balance_usdt + (GlobalVariables::accounts[robot_seller.account_id].balance_usdt - GlobalVariables::accounts[robot_buyer.account_id].balance_usdt) / 2.0;
			walletHistory_buyer.coin_id = coin->coin_id;
			walletHistory_buyer.coinBefore_qty = coin->wallets[robot_buyer.account_id].quantity;
			walletHistory_buyer.coinAfter_qty = coin->wallets[robot_buyer.account_id].quantity + (coin->wallets[robot_seller.account_id].quantity - coin->wallets[robot_buyer.account_id].quantity) / 2.0;
			walletHistory_buyer.time = time(0);

			WalletHistory walletHistory_seller;
			walletHistory_seller.account_id = robot_seller.account_id;
			walletHistory_seller.balanceBefore_usd = GlobalVariables::accounts[robot_seller.account_id].balance_usdt;
			walletHistory_seller.balanceAfter_usd = GlobalVariables::accounts[robot_seller.account_id].balance_usdt + (GlobalVariables::accounts[robot_buyer.account_id].balance_usdt - GlobalVariables::accounts[robot_seller.account_id].balance_usdt) / 2.0;
			walletHistory_seller.coin_id = coin->coin_id;
			walletHistory_seller.coinBefore_qty = coin->wallets[robot_seller.account_id].quantity;
			walletHistory_seller.coinAfter_qty = coin->wallets[robot_seller.account_id].quantity + (coin->wallets[robot_buyer.account_id].quantity - coin->wallets[robot_seller.account_id].quantity) / 2.0;
			walletHistory_seller.time = time(0);
			//*******************************************************************************************************************

			if (!Database_manager::walletHistory_dbt.Insert(walletHistory_buyer, UtilityTools::Set_wallet_quantity(robot_buyer.account_id, coin->coin_id, walletHistory_buyer.coinAfter_qty)))
				return res;
			GlobalVariables::accounts[robot_buyer.account_id].balance_usdt = walletHistory_buyer.balanceAfter_usd;

			if (!Database_manager::walletHistory_dbt.Insert(walletHistory_seller, UtilityTools::Set_wallet_quantity(robot_seller.account_id, coin->coin_id, walletHistory_seller.coinAfter_qty)))
				return res;
			GlobalVariables::accounts[robot_seller.account_id].balance_usdt = walletHistory_seller.balanceAfter_usd;

			res = true;
		}
	}
	CATCH_STATEMENT

	return res;
}

void TradeRequestProcessor::Get_bestSecondTradeRequestID(side_enum tradeRequest_side, int &bestTradeRequestID, int &secondTradeRequestID)
{
	try
	{
		for (map<int, TradeRequest>::iterator it_tradeRequest = tradeRequests.begin();
			 it_tradeRequest != tradeRequests.end();
			 it_tradeRequest++)
		{
			if (!it_tradeRequest->second.mustDelete && it_tradeRequest->second.type == type_enum::Limit)
			{
				if (it_tradeRequest->second.get_side_OmsRegion() == tradeRequest_side)
				{
					if (bestTradeRequestID == 0 ||
						(it_tradeRequest->second.get_price_OmsRegion() < tradeRequests[bestTradeRequestID].get_price_OmsRegion() && tradeRequest_side == side_enum::Sell) ||
						(it_tradeRequest->second.get_price_OmsRegion() > tradeRequests[bestTradeRequestID].get_price_OmsRegion() && tradeRequest_side == side_enum::Buy))
					{
						secondTradeRequestID = bestTradeRequestID;
						bestTradeRequestID = it_tradeRequest->first;
					}
					else if (secondTradeRequestID == 0 ||
							 (it_tradeRequest->second.get_price_OmsRegion() < tradeRequests[secondTradeRequestID].get_price_OmsRegion() && tradeRequest_side == side_enum::Sell) ||
							 (it_tradeRequest->second.get_price_OmsRegion() > tradeRequests[secondTradeRequestID].get_price_OmsRegion() && tradeRequest_side == side_enum::Buy))
					{
						secondTradeRequestID = it_tradeRequest->first;
					}
				}
			}
		}
	}
	CATCH_STATEMENT
}

int TradeRequestProcessor::Get_DistanceFromBestRequest(int tradeRequest_id, side_enum tradeRequest_side)
{
	int distance = 0;
	try
	{
		for (map<int, TradeRequest>::iterator it_tradeRequest = tradeRequests.begin();
			 it_tradeRequest != tradeRequests.end();
			 it_tradeRequest++)
		{
			if (!it_tradeRequest->second.mustDelete)
			{
				if (it_tradeRequest->second.get_side_OmsRegion() == tradeRequest_side)
				{
					if ((it_tradeRequest->second.get_price_OmsRegion() < tradeRequests[tradeRequest_id].get_price_OmsRegion() && tradeRequest_side == side_enum::Sell) ||
						(it_tradeRequest->second.get_price_OmsRegion() > tradeRequests[tradeRequest_id].get_price_OmsRegion() && tradeRequest_side == side_enum::Buy))
					{
						distance++;
					}
				}
			}
		}
	}
	CATCH_STATEMENT

	return distance;
}

void TradeRequestProcessor::RobotBuyer_TradeRequest_queue()
{
	try
	{
		if (coin->tradeRequestRobotID_buyer != 0)
		{
			if (GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer].enable)
			{
				TradeRequestRobot robot = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_buyer];
				int bestTradeRequestID_buy = 0;
				int secondTradeRequestID_buy = 0;
				int bestTradeRequestID_sell = 0;
				int secondTradeRequestID_sell = 0;
				Get_bestSecondTradeRequestID(side_enum::Buy, bestTradeRequestID_buy, secondTradeRequestID_buy);
				Get_bestSecondTradeRequestID(side_enum::Sell, bestTradeRequestID_sell, secondTradeRequestID_sell);

				if (bestTradeRequestID_buy != 0 && bestTradeRequestID_sell != 0)
				{
					if (secondTradeRequestID_sell != 0 && tradeRequests[bestTradeRequestID_sell].status == status_enum::Active &&
						((tradeRequests[secondTradeRequestID_sell].get_price_OmsRegion() - tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion()) > (tradeRequests[secondTradeRequestID_sell].get_price_OmsRegion() - tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion()) * robot.tradeWithBestReq_minPriceDiffSecondReqFromOppositeBestReq_perc) &&
						!IsRobotAccount(tradeRequests[bestTradeRequestID_sell].account_id) &&
						(time(0) - tradeRequests[bestTradeRequestID_sell].time) > (int)UtilityTools::Random(robot.actionDelay_min_sec, robot.actionDelay_max_sec))
					{
						if (tradeRequests[bestTradeRequestID_buy].time < tradeRequests[bestTradeRequestID_sell].time)
						{
							TradeRequest tr_new_buy;
							tr_new_buy.account_id = robot.account_id;
							tr_new_buy.coin_id = coin->coin_id;
							tr_new_buy.set_new_price_OmsRegion(tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion());
							tr_new_buy.set_new_quantity_OmsRegion(tradeRequests[bestTradeRequestID_sell].get_quantity_OmsRegion());
							tr_new_buy.set_side_DbRegion(side_enum::Buy);
							tr_new_buy.status = status_enum::Active;
							tr_new_buy.type = type_enum::Limit;
							tr_new_buy.stopOrder2MarketOrder_Converted = false;
							tr_new_buy.coinPair = coin->name;
							tr_new_buy.visible = true;
							tr_new_buy.time = time(0);

							if ((tr_new_buy.get_quantity_OmsRegion() * tr_new_buy.get_price_OmsRegion()) <= GlobalVariables::accounts[robot.account_id].balance_usdt &&
								GlobalVariables::accounts[robot.account_id].balance_usdt > get_minimumAllowedTradeAmount())
							{
								Database_manager::tradeRequest_dbt.Insert(tr_new_buy);
								// LogOutInsertDeleteRobotTradeRequest("Insert__buy_2", tr_new_buy.tradeRequest_id, bestTradeRequestID_buy, bestTradeRequestID_sell, false);
							}
						}
					}
					else
					{
						if (robotBuyerTRs_buy.size() == 0 || (robotBuyerTRs_buy.size() < robot.maxQueueLength && bestTradeRequestID_buy != robotBuyerTRs_buy.back()))
						{
							Generate_OneRobotTradeRequest_buy(robot, bestTradeRequestID_buy, bestTradeRequestID_sell);
						}
						else
						{
							if (Get_DistanceFromBestRequest(robotBuyerTRs_buy.front(), side_enum::Buy) > robot.KeepOrder_maxDistanceFromBestRequest_num ||
								(tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion() - tradeRequests[robotBuyerTRs_buy.front()].get_price_OmsRegion()) >
									(robot.KeepOrder_maxPriceDifferenceFromBestRequest_perc * tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion()) ||
								(time(0) - tradeRequests[robotBuyerTRs_buy.front()].time) > robot.KeepOrder_maxTime_sec)
							{
								// LogOutInsertDeleteRobotTradeRequest("Delete__buy_1", robotBuyerTRs_buy.front(), bestTradeRequestID_buy, bestTradeRequestID_sell, true);
								Database_manager::tradeRequest_dbt.Delete(robotBuyerTRs_buy.front(), tradeRequests);
								robotBuyerTRs_buy.erase(robotBuyerTRs_buy.begin());
							}
							else if (time(0) - tradeRequests[robotBuyerTRs_buy.back()].time > (int)UtilityTools::Random(robot.actionDelay_min_sec, robot.actionDelay_max_sec))
							{
								// LogOutInsertDeleteRobotTradeRequest("Delete__buy_2", robotBuyerTRs_buy.back(), bestTradeRequestID_buy, bestTradeRequestID_sell, true);
								Database_manager::tradeRequest_dbt.Delete(robotBuyerTRs_buy.back(), tradeRequests);
								robotBuyerTRs_buy.pop_back();
							}
						}
					}
				}
			}
			else
			{
				while (robotBuyerTRs_buy.size() > 0)
				{
					// LogOutInsertDeleteRobotTradeRequest("Delete__buy_3", robotBuyerTRs_buy.back(), 0, 0, true);
					Database_manager::tradeRequest_dbt.Delete(robotBuyerTRs_buy.back(), tradeRequests);
					robotBuyerTRs_buy.pop_back();
				}
			}
		}
	}
	CATCH_STATEMENT
}

void TradeRequestProcessor::RobotSeller_TradeRequest_queue()
{
	try
	{
		if (coin->tradeRequestRobotID_seller != 0)
		{
			if (GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller].enable)
			{
				TradeRequestRobot robot = GlobalVariables::tradeRequestRobots[coin->tradeRequestRobotID_seller];
				int bestTradeRequestID_buy = 0;
				int secondTradeRequestID_buy = 0;
				int bestTradeRequestID_sell = 0;
				int secondTradeRequestID_sell = 0;
				Get_bestSecondTradeRequestID(side_enum::Buy, bestTradeRequestID_buy, secondTradeRequestID_buy);
				Get_bestSecondTradeRequestID(side_enum::Sell, bestTradeRequestID_sell, secondTradeRequestID_sell);

				if (bestTradeRequestID_sell != 0 && bestTradeRequestID_buy != 0)
				{
					if (secondTradeRequestID_buy != 0 && tradeRequests[bestTradeRequestID_buy].status == status_enum::Active &&
						((tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion() - tradeRequests[secondTradeRequestID_buy].get_price_OmsRegion()) > (tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion() - tradeRequests[secondTradeRequestID_buy].get_price_OmsRegion()) * robot.tradeWithBestReq_minPriceDiffSecondReqFromOppositeBestReq_perc) &&
						!IsRobotAccount(tradeRequests[bestTradeRequestID_buy].account_id) &&
						(time(0) - tradeRequests[bestTradeRequestID_buy].time) > (int)UtilityTools::Random(robot.actionDelay_min_sec, robot.actionDelay_max_sec))
					{
						if (tradeRequests[bestTradeRequestID_buy].time > tradeRequests[bestTradeRequestID_sell].time)
						{
							TradeRequest tr_new_sell;
							tr_new_sell.account_id = robot.account_id;
							tr_new_sell.coin_id = coin->coin_id;
							tr_new_sell.set_new_price_OmsRegion(tradeRequests[bestTradeRequestID_buy].get_price_OmsRegion());
							tr_new_sell.set_new_quantity_OmsRegion(tradeRequests[bestTradeRequestID_buy].get_quantity_OmsRegion());
							tr_new_sell.set_side_DbRegion(side_enum::Sell);
							tr_new_sell.status = status_enum::Active;
							tr_new_sell.type = type_enum::Limit;
							tr_new_sell.stopOrder2MarketOrder_Converted = false;
							tr_new_sell.coinPair = coin->name;
							tr_new_sell.visible = true;
							tr_new_sell.time = time(0);

							if (coin->wallets.find(robot.account_id) != coin->wallets.end() &&
								tr_new_sell.get_quantity_OmsRegion() <= coin->wallets[robot.account_id].quantity &&
								(coin->wallets[robot.account_id].quantity * tr_new_sell.get_price_OmsRegion()) > get_minimumAllowedTradeAmount())
							{
								Database_manager::tradeRequest_dbt.Insert(tr_new_sell);
								// LogOutInsertDeleteRobotTradeRequest("Insert_sell_2", tr_new_sell.tradeRequest_id, bestTradeRequestID_buy, bestTradeRequestID_sell, false);
							}
						}
					}
					else
					{
						if (robotSellerTRs_sell.size() == 0 || (robotSellerTRs_sell.size() < robot.maxQueueLength && bestTradeRequestID_sell != robotSellerTRs_sell.back()))
						{
							Generate_OneRobotTradeRequest_sell(robot, bestTradeRequestID_buy, bestTradeRequestID_sell);
						}
						else
						{
							if (Get_DistanceFromBestRequest(robotSellerTRs_sell.front(), side_enum::Sell) > robot.KeepOrder_maxDistanceFromBestRequest_num ||
								(tradeRequests[robotSellerTRs_sell.front()].get_price_OmsRegion() - tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion()) >
									(robot.KeepOrder_maxPriceDifferenceFromBestRequest_perc * tradeRequests[bestTradeRequestID_sell].get_price_OmsRegion()) ||
								(time(0) - tradeRequests[robotSellerTRs_sell.front()].time) > robot.KeepOrder_maxTime_sec)
							{
								// LogOutInsertDeleteRobotTradeRequest("Delete_sell_1", robotSellerTRs_sell.front(), bestTradeRequestID_buy, bestTradeRequestID_sell, true);
								Database_manager::tradeRequest_dbt.Delete(robotSellerTRs_sell.front(), tradeRequests);
								robotSellerTRs_sell.erase(robotSellerTRs_sell.begin());
							}
							else if (time(0) - tradeRequests[robotSellerTRs_sell.back()].time > (int)UtilityTools::Random(robot.actionDelay_min_sec, robot.actionDelay_max_sec))
							{
								// LogOutInsertDeleteRobotTradeRequest("Delete_sell_2", robotSellerTRs_sell.back(), bestTradeRequestID_buy, bestTradeRequestID_sell, true);
								Database_manager::tradeRequest_dbt.Delete(robotSellerTRs_sell.back(), tradeRequests);
								robotSellerTRs_sell.pop_back();
							}
						}
					}
				}
			}
			else
			{
				while (robotSellerTRs_sell.size() > 0)
				{
					// LogOutInsertDeleteRobotTradeRequest("Delete_sell_3", robotSellerTRs_sell.back(), 0, 0, true);
					Database_manager::tradeRequest_dbt.Delete(robotSellerTRs_sell.back(), tradeRequests);
					robotSellerTRs_sell.pop_back();
				}
			}
		}
	}
	CATCH_STATEMENT
}