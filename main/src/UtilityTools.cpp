/**
 * @file UtilityTools.cpp
 * @brief Implementation of the UtilityTools class for common utility functions
 *
 * This file contains the implementation of the UtilityTools class which provides
 * a collection of static helper functions for various common tasks including
 * mathematical operations, string manipulation, date/time conversion, and
 * data transformations.
 */

#include "UtilityTools.h"
#include "GlobalVariables.h"

/**
 * @brief Generates a random number within a specified range
 *
 * @param fMin Minimum value for the random number
 * @param fMax Maximum value for the random number
 * @return double Random number between fMin and fMax (inclusive)
 *
 * @note Uses standard C++ rand() function
 * @note Returns floating-point number within the specified range
 */
double UtilityTools::Random(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;

	return fMin + f * (fMax - fMin);
}

/**
 * @brief Rounds a value down to specified decimal precision
 *
 * @param value Value to round down
 * @param decimalPrecision Number of decimal places to round to
 * @return double Value rounded down to specified precision
 *
 * @note Uses mathematical floor operation with precision scaling
 * @note Example: Floor(3.14159, 2) returns 3.14
 */
double UtilityTools::Floor(double value, int decimalPrecision)
{
	double coef = std::pow(10, decimalPrecision);
	return floor(value * coef) / coef;
}

/**
 * @brief Rounds a value up to specified decimal precision
 *
 * @param value Value to round up
 * @param decimalPrecision Number of decimal places to round to
 * @return double Value rounded up to specified precision
 *
 * @note Uses mathematical ceiling operation with precision scaling
 * @note Example: Ceil(3.14159, 2) returns 3.15
 */
double UtilityTools::Ceil(double value, int decimalPrecision)
{
	double coef = std::pow(10, decimalPrecision);
	return ceil(value * coef) / coef;
}

/**
 * @brief Trims characters from the left side of a string
 *
 * @param s Input string to trim
 * @param trim Characters to remove from the left
 * @return std::string String with left-side characters removed
 *
 * @note Removes all characters in trim string from the beginning
 * @note Returns empty string if all characters are trimmed
 */
std::string UtilityTools::string_left_trim(const std::string &s, std::string trim)
{
	size_t start = s.find_first_not_of(trim);
	return (start == std::string::npos) ? "" : s.substr(start);
}

/**
 * @brief Trims characters from the right side of a string
 *
 * @param s Input string to trim
 * @param trim Characters to remove from the right
 * @return std::string String with right-side characters removed
 *
 * @note Removes all characters in trim string from the end
 * @note Returns empty string if all characters are trimmed
 */
std::string UtilityTools::string_right_trim(const std::string &s, std::string trim)
{
	size_t end = s.find_last_not_of(trim);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

/**
 * @brief Attaches a string to the end of another string
 *
 * @param s Base string
 * @param endPart String to attach to the end
 * @return std::string Concatenated string
 *
 * @note Simple string concatenation operation
 * @note Returns new string with endPart appended
 */
std::string UtilityTools::string_attachEnd(const std::string &s, std::string endPart)
{
	string res = s;
	res += endPart;

	return res;
}

/**
 * @brief Erases a substring from a string
 *
 * @param mainStr Main string to modify
 * @param toErase Substring to remove
 * @return string String with substring removed
 *
 * @note Removes first occurrence of toErase from mainStr
 * @note Returns original string if substring not found
 */
string UtilityTools::string_eraseSubStr(std::string mainStr, const std::string &toErase)
{
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}

	return mainStr;
}

/**
 * @brief Trims characters from both sides of a string
 *
 * @param s Input string to trim
 * @param trim_left Characters to remove from the left
 * @param trim_right Characters to remove from the right
 * @return std::string String with both sides trimmed
 *
 * @note Applies left trim followed by right trim
 * @note Returns string with specified characters removed from both ends
 */
std::string UtilityTools::string_trim(const std::string &s, std::string trim_left, std::string trim_right)
{
	return string_right_trim(string_left_trim(s, trim_left), trim_right);
}

/**
 * @brief Calculates the difference in days between two timestamps
 *
 * @param DateA First timestamp
 * @param DateB Second timestamp
 * @return int Number of days difference (DateA - DateB)
 *
 * @note Calculates difference in seconds and converts to days
 * @note Returns positive value if DateA is later than DateB
 * @note Returns negative value if DateA is earlier than DateB
 */
int UtilityTools::DateDaySubtraction(time_t DateA, time_t DateB)
{
	// tm* tmA = localtime(&DateA);
	// tm* tmB = localtime(&DateB);
	// int	result = (tmA->tm_year - tmB->tm_year) * 365 + tmA->tm_yday - tmB->tm_yday;

	int result = (DateA - DateB) / (60 * 60 * 24);

	return result;
}

/**
 * @brief Converts a timestamp to a formatted datetime string
 *
 * @param timestamp Unix timestamp to convert
 * @param return_val_be_sql_format If true, returns SQL-formatted string with quotes
 * @return string Formatted datetime string or "NULL" if timestamp is invalid
 *
 * @note Returns "NULL" for invalid timestamps (<= 0)
 * @note SQL format includes single quotes around the datetime
 * @note Standard format: "YYYY-MM-DD HH:MM:SS"
 * @note SQL format: "'YYYY-MM-DD HH:MM:SS'"
 */
string UtilityTools::Timestamp2DatetimeString(time_t timestamp, bool return_val_be_sql_format)
{
	std::string res = "NULL";

	if (timestamp > 0)
	{
		struct tm tstruct;
		char buf[34];
		tstruct = *localtime(&timestamp);
		if (return_val_be_sql_format)
			strftime(buf, sizeof(buf), "'%Y-%m-%d %X'", &tstruct);
		else
			strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
		res = buf;
	}
	return res;
}

/**
 * @brief Formats price feed data into a message frame
 *
 * @param message Buffer to store the formatted message
 * @param symbol_name Trading symbol name
 * @param bid_price Current bid price
 * @param bid_volume Bid volume
 * @param ask_price Current ask price
 * @param ask_volume Ask volume
 *
 * @note Formats data as: "symbol,0,bid_price,bid_volume,1,ask_price,ask_volume|"
 * @note Uses sprintf for formatting into the provided buffer
 * @note Includes pipe character as message delimiter
 */
void UtilityTools::LocateFeedInMessageFrame(char *message, const char *symbol_name, double bid_price, double bid_volume, double ask_price, double ask_volume)
{
	sprintf(message, "%s,0,%.8f,%.8f,1,%.8f,%.8f|", symbol_name, bid_price, bid_volume, ask_price, ask_volume);
}

/**
 * @brief Converts a datetime string to a Unix timestamp
 *
 * @param s Datetime string in format "YYYY-MM-DD HH:MM:SS"
 * @return time_t Unix timestamp
 *
 * @note Parses datetime string using strptime
 * @note Handles timezone conversion automatically
 * @note Calculates timestamp including leap year adjustments
 * @note Returns timestamp adjusted for local timezone
 */
time_t UtilityTools::DatetimeString2Timestamp(const std::string s)
{
	struct tm tmlol;
	strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &tmlol);

	const std::time_t epoch_plus_11h = 60 * 60 * 11;
	const int local_time = localtime(&epoch_plus_11h)->tm_hour * 3600 + localtime(&epoch_plus_11h)->tm_min * 60;
	const int gm_time = gmtime(&epoch_plus_11h)->tm_hour * 3600 + gmtime(&epoch_plus_11h)->tm_min * 60;
	const int tz_diff = local_time - gm_time;

	time_t t = (tmlol.tm_sec + tmlol.tm_min * 60 + tmlol.tm_hour * 3600 + tmlol.tm_yday * 86400 +
				(tmlol.tm_year - 70) * 31536000 + ((tmlol.tm_year - 69) / 4) * 86400 -
				((tmlol.tm_year - 1) / 100) * 86400 + ((tmlol.tm_year + 299) / 400) * 86400) -
			   tz_diff;

	return t;
}

/**
 * @brief Checks if a string contains only ASCII characters
 *
 * @param s String to check
 * @return bool True if string contains only ASCII characters, false otherwise
 *
 * @note Uses std::any_of with lambda to check for non-ASCII characters
 * @note Returns false if any character has value > 127
 * @note Returns true for empty strings
 */
bool UtilityTools::StringIsASCII(const std::string &s)
{
	return !std::any_of(s.begin(), s.end(), [](char c)
						{ return static_cast<unsigned char>(c) > 127; });
}

/**
 * @brief Retrieves wallet quantity for a specific account and coin
 *
 * @param account_id ID of the account
 * @param coin_id ID of the cryptocurrency
 * @return double Wallet quantity for the specified account and coin
 *
 * @note Returns 0 if wallet not found or on error
 * @note Thread-safe operation with exception handling
 * @note Logs errors for debugging purposes
 * @note Accesses global coin and wallet data structures
 */
double UtilityTools::Get_wallet_quantity(int account_id, int coin_id)
{
	double res = 0;
	try
	{
		Coin *coin = &GlobalVariables::coins[coin_id];
		map<int, Wallet>::iterator it = coin->wallets.find(account_id);
		if (it != coin->wallets.end())
		{
			res = it->second.quantity;
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

/**
 * @brief Sets wallet quantity for a specific account and coin
 *
 * @param account_id ID of the account
 * @param coin_id ID of the cryptocurrency
 * @param coin_qty New quantity to set
 * @return bool True if quantity was set successfully, false otherwise
 *
 * @note Returns false if wallet not found or on error
 * @note Thread-safe operation with exception handling
 * @note Logs errors for debugging purposes
 * @note Modifies global coin and wallet data structures
 */
bool UtilityTools::Set_wallet_quantity(int account_id, int coin_id, double coin_qty)
{
	bool res = false;
	try
	{
		Coin *coin = &GlobalVariables::coins[coin_id];
		map<int, Wallet>::iterator it = coin->wallets.find(account_id);
		if (it != coin->wallets.end())
		{
			it->second.quantity = coin_qty;
			res = true;
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

/**
 * @brief Sets USDT balance for a specific account
 *
 * @param account_id ID of the account
 * @param balance_usdt New USDT balance to set
 * @return bool True if balance was set successfully, false otherwise
 *
 * @note Returns false on error
 * @note Thread-safe operation with exception handling
 * @note Logs errors for debugging purposes
 * @note Modifies global account data structures
 */
bool UtilityTools::Set_account_balance_usdt(int account_id, double balance_usdt)
{
	bool res = false;
	try
	{
		GlobalVariables::accounts[account_id].balance_usdt = balance_usdt;
		res = true;
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

/**
 * @brief Converts trading side from OMS region to database region
 *
 * @param side_OmsRegion Trading side in OMS region format
 * @param coin_id ID of the cryptocurrency
 * @return side_enum Trading side in database region format
 *
 * @note For USDT-based coins, side remains the same
 * @note For non-USDT-based coins, side is inverted (Buy becomes Sell, Sell becomes Buy)
 * @note Handles different coin base currency configurations
 */
side_enum UtilityTools::Get_side_DbRegion(side_enum side_OmsRegion, int coin_id)
{
	side_enum buySellSide_DbRegion;
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		buySellSide_DbRegion = side_OmsRegion;
	}
	else
	{
		buySellSide_DbRegion = side_OmsRegion == side_enum::Buy ? side_enum::Sell : side_enum::Buy;
	}
	return buySellSide_DbRegion;
}

/**
 * @brief Converts trading side from database region to OMS region
 *
 * @param side_DbRegion Trading side in database region format
 * @param coin_id ID of the cryptocurrency
 * @return side_enum Trading side in OMS region format
 *
 * @note For USDT-based coins, side remains the same
 * @note For non-USDT-based coins, side is inverted (Buy becomes Sell, Sell becomes Buy)
 * @note Handles different coin base currency configurations
 */
side_enum UtilityTools::Get_side_OmsRegion(side_enum side_DbRegion, int coin_id)
{
	side_enum buySellSide_OmsRegion;
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		buySellSide_OmsRegion = side_DbRegion;
	}
	else
	{
		buySellSide_OmsRegion = side_DbRegion == side_enum::Buy ? side_enum::Sell : side_enum::Buy;
	}
	return buySellSide_OmsRegion;
}

/**
 * @brief Converts price from OMS region to database region
 *
 * @param price_OmsRegion Price in OMS region format
 * @param coin_id ID of the cryptocurrency
 * @return double Price in database region format
 *
 * @note For USDT-based coins, price remains the same
 * @note For non-USDT-based coins, price is inverted (1/price)
 * @note Returns 0 if input price is 0
 * @note Handles different coin base currency configurations
 */
double UtilityTools::Get_price_DbRegion(double price_OmsRegion, int coin_id)
{
	double price_DbRegion = 0;
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		price_DbRegion = price_OmsRegion;
	}
	else
	{
		if (price_OmsRegion != 0)
			price_DbRegion = 1 / price_OmsRegion;
		else
			price_DbRegion = 0;
	}
	return price_DbRegion;
}

/**
 * @brief Converts quantity from OMS region to database region
 *
 * @param quantity_OmsRegion Quantity in OMS region format
 * @param price_OmsRegion Price in OMS region format
 * @param coin_id ID of the cryptocurrency
 * @return double Quantity in database region format
 *
 * @note For USDT-based coins, quantity remains the same
 * @note For non-USDT-based coins, quantity is multiplied by price
 * @note Handles different coin base currency configurations
 */
double UtilityTools::Get_quantity_DbRegion(double quantity_OmsRegion, double price_OmsRegion, int coin_id)
{
	double quantity_DbRegion = 0;
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		quantity_DbRegion = quantity_OmsRegion;
	}
	else
	{
quantity_DbRegion = quantity_OmsRegion*price_OmsRegion;

		// double price_DbRegion = 0;
		// if (price_OmsRegion != 0)
		// {
		// 	price_DbRegion = Get_price_DbRegion(price_OmsRegion, coin_id);
		// }

		// if (price_DbRegion != 0)
		// 	quantity_DbRegion = quantity_OmsRegion / price_DbRegion;
		// else
		// 	quantity_DbRegion = 0;
	}
	return quantity_DbRegion;
}

/**
 * @brief Converts quantity from database region to OMS region
 *
 * @param quantity_DbRegion Quantity in database region format
 * @param price_DbRegion Price in database region format
 * @param coin_id ID of the cryptocurrency
 * @return double Quantity in OMS region format
 *
 * @note For USDT-based coins, quantity remains the same
 * @note For non-USDT-based coins, quantity is multiplied by price
 * @note Handles different coin base currency configurations
 */
double UtilityTools::Get_quantity_OmsRegion(double quantity_DbRegion, double price_DbRegion, int coin_id)
{
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		return quantity_DbRegion;
	}
	else
	{
		return quantity_DbRegion * price_DbRegion;
	}
}

/**
 * @brief Converts price from database region to OMS region
 *
 * @param price_DbRegion Price in database region format
 * @param coin_id ID of the cryptocurrency
 * @return double Price in OMS region format
 *
 * @note For USDT-based coins, price remains the same
 * @note For non-USDT-based coins, price is inverted (1/price)
 * @note Returns 0 if input price is 0
 * @note Handles different coin base currency configurations
 */
double UtilityTools::Get_price_OmsRegion(double price_DbRegion, int coin_id)
{
	double price_OmsRegion = 0;
	if (GlobalVariables::coins[coin_id].is_usdtBase)
	{
		price_OmsRegion = price_DbRegion;
	}
	else
	{
		if (price_DbRegion != 0)
			price_OmsRegion = 1 / price_DbRegion;
		else
			price_OmsRegion = 0;
	}
	return price_OmsRegion;
}

/**
 * @brief Checks if a coin is USDT-based
 *
 * @param coin_id ID of the cryptocurrency
 * @return bool True if coin is USDT-based, false otherwise
 *
 * @note Accesses global coin configuration data
 * @note Returns the is_usdtBase flag for the specified coin
 */
bool UtilityTools::Coin_is_UsdtBase(int coin_id)
{
	return GlobalVariables::coins[coin_id].is_usdtBase;
}
/**
 * @brief Converts a string to lowercase
 *
 * @param SrcString Source string to convert
 * @return string String converted to lowercase
 *
 * @note Uses std::transform with std::tolower
 * @note Converts all characters to lowercase
 * @note Returns new string with lowercase characters
 */
string UtilityTools::Get_LowerCharString(string SrcString)
{
	std::transform(SrcString.begin(), SrcString.end(), SrcString.begin(), [](unsigned char c)
				   { return std::tolower(c); });

	return SrcString;
}
