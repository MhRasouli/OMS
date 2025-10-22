#include "BinanceTrader.h"
#include "UtilityTools.h"
#include "Database_manager.h"

BinanceTrader::BinanceTrader(Coin *_coin) : MarketTrader(_coin)
{
	api = new binapi::rest::api(
		ioctx, GlobalVariables::markets[coin->market_id].host_trade, to_string(GlobalVariables::markets[coin->market_id].port_trade), GlobalVariables::markets[coin->market_id].username_privateKey, GlobalVariables::markets[coin->market_id].password_securityKey, 10000);
}

bool BinanceTrader::MarketOrder(int tradeRequest_id, side_enum side, double quantity, double commission_qty, double price, int coin_id, double &qty_res, double &price_res, MarketResponse &marketResponse, string &rejectReason)
{
	string str_precision = "%.";
	str_precision += to_string(coin->quantity_precision);
	str_precision += "f";

	bool result = false;

	if (side == side_enum::Sell)
		quantity = quantity - commission_qty, price;

	binapi::rest::api::result<binapi::rest::new_order_resp_type> neworder = api->new_order(
		UtilityTools::string_attachEnd(coin->name, "USDT").c_str(), side == side_enum::Buy ? binapi::e_side::buy : binapi::e_side::sell, binapi::e_type::market, binapi::e_time::GTC, binapi::e_trade_resp_type::FULL, UtilityTools::StringFormat(str_precision.c_str(), UtilityTools::Floor(quantity, coin->quantity_precision)).c_str(), nullptr, nullptr, nullptr, nullptr);
	if (neworder.ec)
	{
		Logger::logger.log_special(LogLocation, true, "Binance order failed. ec=%i, errmsg=%s", neworder.ec, neworder.errmsg.c_str());
		rejectReason = neworder.errmsg;
		result = false;
	}
	else
	{
		binapi::rest::new_order_info_full_t NOI_full = neworder.v.get_responce_full();
		marketResponse.symbol = NOI_full.symbol;
		marketResponse.orderId = to_string(NOI_full.orderId);
		marketResponse.clientOrderId = NOI_full.clientOrderId;
		marketResponse.transactTime = NOI_full.transactTime;
		marketResponse.price = NOI_full.price.convert_to<double>();
		marketResponse.origQty = NOI_full.origQty.convert_to<double>();
		marketResponse.executedQty = NOI_full.executedQty.convert_to<double>();
		marketResponse.cummulativeQuoteQty = NOI_full.cummulativeQuoteQty.convert_to<double>();
		marketResponse.status = NOI_full.status;
		marketResponse.timeInForce = NOI_full.timeInForce;
		marketResponse.type = NOI_full.type;
		marketResponse.side = NOI_full.side;
		marketResponse.orderListId = 0;

		for (vector<binapi::rest::new_order_info_full_t::fill_part>::iterator it = NOI_full.fills.begin();
			 it != NOI_full.fills.end(); it++)
		{
			MarketResponseDetail marketResponseDetail;
			marketResponseDetail.price = it->price.convert_to<double>();
			marketResponseDetail.qty = it->qty.convert_to<double>();
			marketResponseDetail.commission = it->commission.convert_to<double>();
			marketResponseDetail.commissionAsset = it->commissionAsset;
			marketResponseDetail.tradeId = 0;

			marketResponse.lst_marketResponseDetail.push_back(marketResponseDetail);
		}
		result = true;
	}

	if (result)
	{
		if (Database_manager::marketResponse_dbt.Insert(marketResponse))
		{
			if (side == side_enum::Buy)
			{
				double gotten_qty = 0;
				for (vector<MarketResponseDetail>::iterator it = marketResponse.lst_marketResponseDetail.begin();
					 it != marketResponse.lst_marketResponseDetail.end(); it++)
				{
					gotten_qty += it->qty - it->commission;
					it->marketResponse_id = marketResponse.marketResponse_id;
					if (!Database_manager::marketResponseDetail_dbt.Insert(*it))
						return false;
				}
				qty_res = gotten_qty;
				price_res = marketResponse.cummulativeQuoteQty / marketResponse.executedQty;
			}
			else
			{
				double gotten_usdt = 0;
				for (vector<MarketResponseDetail>::iterator it = marketResponse.lst_marketResponseDetail.begin();
					 it != marketResponse.lst_marketResponseDetail.end(); it++)
				{
					gotten_usdt += (it->price * it->qty) - it->commission;
					it->marketResponse_id = marketResponse.marketResponse_id;
					if (!Database_manager::marketResponseDetail_dbt.Insert(*it))
						return false;
				}

				qty_res = marketResponse.executedQty;
				price_res = gotten_usdt / marketResponse.executedQty;
			}
		}
	}

	return result;
}