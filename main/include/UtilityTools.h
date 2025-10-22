#pragma once

#include "stdafx.h"
#include <ctime>
#include <memory>
#include <string>
#include <stdexcept>
#include "GlobalVariables.h"

using namespace std;

/**
 * @class UtilityTools
 * @brief Utility class providing common helper functions for the trading service
 * 
 * This class contains a collection of static utility functions used throughout
 * the trading service for common operations such as string manipulation,
 * date/time conversion, mathematical operations, and data region conversions.
 * 
 * Features:
 * - String manipulation and formatting utilities
 * - Date and time conversion functions
 * - Mathematical operations (random, floor, ceil)
 * - Data region conversion (OMS ↔ Database)
 * - Wallet and account balance management
 * - Feed message parsing and processing
 * - ASCII string validation
 * 
 * All methods are static and can be called without instantiating the class.
 * These utilities provide essential functionality for data processing,
 * formatting, and conversion throughout the trading system.
 * 
 * @note All methods are static for direct access
 * @note Provides comprehensive utility functions for the trading service
 * @note Includes template methods for flexible string formatting
 */
class UtilityTools
{
public:
	/**
	 * @brief Locates and processes feed data in a message frame
	 * 
	 * @param message Pointer to the message buffer
	 * @param symbol_name Symbol name to locate in the message
	 * @param bid_price Reference to store bid price
	 * @param bid_volume Reference to store bid volume
	 * @param ask_price Reference to store ask price
	 * @param ask_volume Reference to store ask volume
	 */
	static void LocateFeedInMessageFrame(char *message, const char *symbol_name, double bid_price, double bid_volume, double ask_price, double ask_volume);
	
	/**
	 * @brief Erases a substring from a main string
	 * 
	 * @param mainStr The main string to modify
	 * @param toErase The substring to remove
	 * @return string Modified string with substring removed
	 */
	static string string_eraseSubStr(std::string mainStr, const std::string &toErase);
	
	/**
	 * @brief Calculates the difference in days between two dates
	 * 
	 * @param DateA First date (timestamp)
	 * @param DateB Second date (timestamp)
	 * @return int Number of days difference
	 */
	static int DateDaySubtraction(time_t DateA, time_t DateB);
	
	/**
	 * @brief Converts timestamp to datetime string
	 * 
	 * @param timestamp Unix timestamp to convert
	 * @param return_val_be_sql_format Whether to return SQL format (default: true)
	 * @return string Formatted datetime string
	 */
	static string Timestamp2DatetimeString(time_t timestamp, bool return_val_be_sql_format = true);
	
	/**
	 * @brief Converts datetime string to timestamp
	 * 
	 * @param s Datetime string to convert
	 * @return time_t Unix timestamp
	 */
	static time_t DatetimeString2Timestamp(const std::string s);
	
	/**
	 * @brief Generates a random number within a range
	 * 
	 * @param fMin Minimum value
	 * @param fMax Maximum value
	 * @return double Random number in the specified range
	 */
	static double Random(double fMin, double fMax);
	
	/**
	 * @brief Rounds down a value to specified decimal precision
	 * 
	 * @param value Value to round down
	 * @param decimalPrecision Number of decimal places
	 * @return double Floored value
	 */
	static double Floor(double value, int decimalPrecision);
	
	/**
	 * @brief Rounds up a value to specified decimal precision
	 * 
	 * @param value Value to round up
	 * @param decimalPrecision Number of decimal places
	 * @return double Ceiled value
	 */
	static double Ceil(double value, int decimalPrecision);

	/**
	 * @brief Template method for string formatting with variadic arguments
	 * 
	 * @param format Format string (printf-style)
	 * @param args Variadic arguments for formatting
	 * @return string Formatted string
	 * 
	 * @note Template method supporting any number of arguments
	 * @note Uses snprintf for safe formatting
	 */
	template <typename... Args>
	static string StringFormat(const string &format, Args... args);
	
	/**
	 * @brief Checks if a string contains only ASCII characters
	 * 
	 * @param s String to check
	 * @return bool True if string is ASCII-only
	 */
	static bool StringIsASCII(const std::string &s);
	
	/**
	 * @brief Attaches a string to the end of another string
	 * 
	 * @param s Main string
	 * @param endPart String to attach at the end
	 * @return std::string Combined string
	 */
	static std::string string_attachEnd(const std::string &s, std::string endPart);
	
	/**
	 * @brief Trims characters from the left side of a string
	 * 
	 * @param s String to trim
	 * @param trim Characters to remove from the left
	 * @return std::string Trimmed string
	 */
	static std::string string_left_trim(const std::string &s, std::string trim);
	
	/**
	 * @brief Trims characters from the right side of a string
	 * 
	 * @param s String to trim
	 * @param trim Characters to remove from the right
	 * @return std::string Trimmed string
	 */
	static std::string string_right_trim(const std::string &s, std::string trim);
	
	/**
	 * @brief Trims characters from both sides of a string
	 * 
	 * @param s String to trim
	 * @param trim_left Characters to remove from the left
	 * @param trim_right Characters to remove from the right
	 * @return std::string Trimmed string
	 */
	static std::string string_trim(const std::string &s, std::string trim_left, std::string trim_right);
	
	/**
	 * @brief Sets wallet quantity for a specific account and coin
	 * 
	 * @param account_id Account ID
	 * @param coin_id Coin ID
	 * @param coin_qty Quantity to set
	 * @return bool True if operation was successful
	 */
	static bool Set_wallet_quantity(int account_id, int coin_id, double coin_qty);
	
	/**
	 * @brief Sets USDT balance for a specific account
	 * 
	 * @param account_id Account ID
	 * @param balance_usdt USDT balance to set
	 * @return bool True if operation was successful
	 */
	static bool Set_account_balance_usdt(int account_id, double balance_usdt);
	
	/**
	 * @brief Gets wallet quantity for a specific account and coin
	 * 
	 * @param account_id Account ID
	 * @param coin_id Coin ID
	 * @return double Current wallet quantity
	 */
	static double Get_wallet_quantity(int account_id, int coin_id);

	/**
	 * @brief Converts side from OMS region to Database region
	 * 
	 * @param side_OmsRegion Side in OMS region
	 * @param coin_id Coin ID for conversion context
	 * @return side_enum Side in Database region
	 */
	static side_enum Get_side_DbRegion(side_enum side_OmsRegion, int coin_id);
	
	/**
	 * @brief Converts side from Database region to OMS region
	 * 
	 * @param side_DbRegion Side in Database region
	 * @param coin_id Coin ID for conversion context
	 * @return side_enum Side in OMS region
	 */
	static side_enum Get_side_OmsRegion(side_enum side_DbRegion, int coin_id);
	
	/**
	 * @brief Converts price from OMS region to Database region
	 * 
	 * @param price_OmsRegion Price in OMS region
	 * @param coin_id Coin ID for conversion context
	 * @return double Price in Database region
	 */
	static double Get_price_DbRegion(double price_OmsRegion, int coin_id);
	
	/**
	 * @brief Converts quantity from OMS region to Database region
	 * 
	 * @param quantity_OmsRegion Quantity in OMS region
	 * @param price_OmsRegion Price in OMS region for conversion
	 * @param coin_id Coin ID for conversion context
	 * @return double Quantity in Database region
	 */
	static double Get_quantity_DbRegion(double quantity_OmsRegion, double price_OmsRegion, int coin_id);
	
	/**
	 * @brief Converts quantity from Database region to OMS region
	 * 
	 * @param quantity_DbRegion Quantity in Database region
	 * @param price_DbRegion Price in Database region for conversion
	 * @param coin_id Coin ID for conversion context
	 * @return double Quantity in OMS region
	 */
	static double Get_quantity_OmsRegion(double quantity_DbRegion, double price_DbRegion, int coin_id);
	
	/**
	 * @brief Converts price from Database region to OMS region
	 * 
	 * @param price_DbRegion Price in Database region
	 * @param coin_id Coin ID for conversion context
	 * @return double Price in OMS region
	 */
	static double Get_price_OmsRegion(double price_DbRegion, int coin_id);

	/**
	 * @brief Checks if a coin is USDT-based
	 * 
	 * @param coin_id Coin ID to check
	 * @return bool True if coin is USDT-based
	 */
	static bool Coin_is_UsdtBase(int coin_id);
	
	/**
	 * @brief Converts a string to lowercase
	 * 
	 * @param SrcString Source string to convert
	 * @return string Lowercase version of the string
	 */
	static string Get_LowerCharString(string SrcString);
};

template <typename... Args>
string UtilityTools::StringFormat(const string &format, Args... args)
{
	int size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
	if (size <= 0)
	{
		throw runtime_error("Error during formatting.");
	}
	unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args...);
	return string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}