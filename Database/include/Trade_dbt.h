#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "Trade.h"
#include "UtilityTools.h"
#include "Coin.h"
#include "AccountTpSl.h"
#include "BuyLimitation.h"

/**
 * @class Trade_dbt
 * @brief Database table class for Trade entity operations
 * 
 * This class provides database operations for the Trade entity including
 * insertion, selection, and update operations. It extends DatabaseTable
 * to provide specific functionality for trade-related database operations.
 * 
 * Features:
 * - Trade insertion and selection operations
 * - Take profit/Stop loss (TP/SL) management
 * - Buyer trade quantity calculations
 * - Active TP/SL trade selection
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides trade-specific database functionality
 * @note Supports TP/SL trade management
 */
class Trade_dbt : public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Trade_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Trade_dbt(DatabaseHandler *databaseHandler);

	/**
	 * @brief Inserts a new trade record into the database
	 * 
	 * @param trade Reference to the Trade object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts trade data into the database
	 * @note Returns false if insertion fails
	 */
	bool Insert(Trade &trade);

	/**
	 * @brief Selects all active take profit/stop loss trades for a specific account and coin
	 * 
	 * @param account_id ID of the account
	 * @param coin_id ID of the coin
	 * @param tradeTpSls Reference to store the list of active TP/SL trades
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves active TP/SL trades for the specified account and coin
	 * @note Populates the tradeTpSls list with results
	 */
	bool SelectAll_AcctiveTpSl(const int account_id,const int coin_id,list<TradeTpSl> &tradeTpSls);

	/**
	 * @brief Updates take profit/stop loss information for a trade
	 * 
	 * @param tradeTpSl TradeTpSl object containing updated TP/SL information
	 * @return bool True if update was successful, false otherwise
	 * 
	 * @note Updates TP/SL information in the database
	 * @note Returns false if update fails
	 */
	bool Update_TpSl(TradeTpSl tradeTpSl);

	/**
	 * @brief Selects the maximum trade quantity for a buyer based on buy limitation
	 * 
	 * @param buyLimitation BuyLimitation object containing limitation rules
	 * @param buyer_id ID of the buyer account
	 * @return double Maximum trade quantity allowed for the buyer
	 * 
	 * @note Calculates maximum trade quantity based on buy limitation rules
	 * @note Returns 0 if no trades found or calculation fails
	 */
	double Select_buyerMaxTrade_qty(BuyLimitation buyLimitation,int buyer_id);
};