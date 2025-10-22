#include "NobitexFeeder.h"
#include "UtilityTools.h"

NobitexFeeder::~NobitexFeeder()
{
	fedchkThread.Stop();
	StopGetData();
}

NobitexFeeder::NobitexFeeder(Coin *Coin_ptr) : coin_ptr(Coin_ptr), fedchkThread(&dataIsNew, Coin_ptr, &mustDelete)
{
	string coinName = coin_ptr->name == "IRT" ? UtilityTools::string_attachEnd("USDT", coin_ptr->name) : UtilityTools::string_attachEnd(coin_ptr->name, "USDT");
	cpr::Url url{UtilityTools::string_attachEnd(GlobalVariables::markets[coin_ptr->market_id].host_feedPrice, coinName).c_str()};

	session.SetUrl(url);
	session.SetAcceptEncoding({{cpr::AcceptEncodingMethods::deflate, cpr::AcceptEncodingMethods::gzip, cpr::AcceptEncodingMethods::zlib}});
}

unsigned NobitexFeeder::ThreadFunction()
{
	fedchkThread.Start("Feed checker symbol: " + coin_ptr->name);
	while (isStopEventSignaled() == false)
	{
		try
		{
			session.PrepareGet();
			CURLcode curl_result = curl_easy_perform(session.GetCurlHolder()->handle);
			cpr::Response response = session.Complete(curl_result);

			// cpr::Response response = session.Get();

			Json::Value root;
			Json::Reader reader;

			reader.parse(response.text, root);

			//*************************************************************************************************

			if (UtilityTools::Coin_is_UsdtBase(coin_ptr->coin_id))
			{
				coin_ptr->market_buy_price = atof(root["asks"][0][0].asString().c_str());
				coin_ptr->market_buy_quantity = atof(root["asks"][0][1].asString().c_str());

				coin_ptr->market_sell_price = atof(root["bids"][0][0].asString().c_str());
				coin_ptr->market_sell_quantity = atof(root["bids"][0][1].asString().c_str());
			}
			else
			{
				coin_ptr->market_sell_price = UtilityTools::Get_price_DbRegion(atof(root["asks"][0][0].asString().c_str()) * coin_ptr->market_price_coefficient, coin_ptr->coin_id);
				coin_ptr->market_sell_quantity = atof(root["asks"][0][1].asString().c_str());

				coin_ptr->market_buy_price = UtilityTools::Get_price_DbRegion(atof(root["bids"][0][0].asString().c_str()) * coin_ptr->market_price_coefficient, coin_ptr->coin_id);
				coin_ptr->market_buy_quantity = atof(root["bids"][0][1].asString().c_str());
			}

			//*************************************************************************************************
			coin_ptr->time_price = time(0);
			dataIsNew = true;
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
		Loop_delay();
	}

	return 0;
}

void NobitexFeeder::StopGetData()
{
	Stop();
}

void NobitexFeeder::Loop_delay()
{
	static time_t time_lastGottentime = time(0);
	do
	{
		this_thread::sleep_for(chrono::milliseconds(50));
	} while (time(0) - time_lastGottentime < 1);

	time_lastGottentime = time(0);
}
