#include "BinanceFeeder.h"
#include "UtilityTools.h"

/*************************************************************************************************/
#define PRINT_IF_ERROR(res)                                                               \
	if (!static_cast<bool>(res))                                                          \
	{                                                                                     \
		std::cout << __FILE__ << "(" << __LINE__ << "): msg=" << res.errmsg << std::endl; \
	}

/*************************************************************************************************/

#define BREAK_IF_ERROR(res)  \
	PRINT_IF_ERROR(res);     \
	if (!res)                \
	{                        \
		return EXIT_FAILURE; \
	}

/*************************************************************************************************/

BinanceFeeder::~BinanceFeeder()
{
	fedchkThread.Stop();
	StopGetData();
}

BinanceFeeder::BinanceFeeder(Coin *Coin_ptr) : coin_ptr(Coin_ptr),
											 wsp(ioctx, GlobalVariables::markets[Coin_ptr->market_id].host_feedPrice, to_string(GlobalVariables::markets[Coin_ptr->market_id].port_feedPrice)),
											 fedchkThread(&dataIsNew, Coin_ptr, &mustDelete)
{
	InitBookTicker();
}

void BinanceFeeder::InitBookTicker()
{
	wsp.book(UtilityTools::string_attachEnd(coin_ptr->name, "USDT").c_str(),
			 [this](const char *fl, int ec, std::string errmsg, binapi::ws::book_ticker_t msg) -> bool
			 {
				 if (ec)
				 {
					 Logger::logger.log_special(LogLocation, true, "subscribe_book(%s): fl=%s, ec=%i, errmsg: %s", coin_ptr->name.c_str(), fl, ec, errmsg.c_str());
					 return false;
				 }

				 coin_ptr->market_buy_price = msg.b.convert_to<double>();
				 coin_ptr->market_buy_quantity = msg.B.convert_to<double>();

				 coin_ptr->market_sell_price = msg.a.convert_to<double>();
				 coin_ptr->market_sell_quantity = msg.A.convert_to<double>();

				 coin_ptr->time_price = time(0);

				 dataIsNew = true;

				 return true;
			 });
}

unsigned BinanceFeeder::ThreadFunction()
{
	fedchkThread.Start("Feed checker symbol: " + coin_ptr->name);
	ioctx.run();

	return 0;
}

void BinanceFeeder::StopGetData()
{
	wsp.async_unsubscribe_all();
	ioctx.stop();
	Stop();
}
