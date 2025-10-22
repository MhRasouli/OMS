#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "TradeRequest.h"
#include "UtilityTools.h"
#include "GlobalVariables.h"
#include "Coin.h"
#include "AccountTpSl.h"

/**
 * @class TradeRequest_dbt
 * @brief Database table class for TradeRequest entity operations
 * 
 * This class provides comprehensive database operations for the TradeRequest entity
 * including selection, insertion, update, and deletion operations. It extends
 * DatabaseTable to provide specific functionality for trade request-related database operations.
 * 
 * Features:
 * - Trade request selection and insertion operations
 * - Generated trade request management
 * - Take profit/Stop loss (TP/SL) operations
 * - Dependent trade request management
 * - Trade request rejection and deletion
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides comprehensive trade request database functionality
 * @note Supports TP/SL and dependent trade request management
 */
class TradeRequest_dbt : public DatabaseTable
{
public:
	/**
	 * @brief Constructor for TradeRequest_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	TradeRequest_dbt(DatabaseHandler *databaseHandler);
	
	/**
	 * @brief Selects a specific trade request by ID
	 * 
	 * @param tradeRequest_id ID of the trade request to select
	 * @param result Reference to store the selected trade request
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves trade request information for the specified ID
	 * @note Populates the result parameter with trade request data
	 */
	bool Select(int tradeRequest_id, TradeRequest &result);
	
	/**
	 * @brief Selects all trade requests for a specific coin
	 * 
	 * @param coin_id ID of the coin
	 * @param TradeRequests Reference to map to store trade requests indexed by trade request ID
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves all trade requests for the specified coin
	 * @note Populates the TradeRequests map with results
	 */
	bool SelectAll(int coin_id, map<int, TradeRequest> &TradeRequests);
	
	/**
	 * @brief Inserts a new trade request and updates the trade requests map
	 * 
	 * @param tradeRequest Reference to the TradeRequest object to insert
	 * @param tradeRequests Reference to map of trade requests to update
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts trade request into database and updates local map
	 * @note Updates the tradeRequests map with the new trade request
	 */
	bool Insert(TradeRequest &tradeRequest, map<int, TradeRequest> &tradeRequests);
	
	/**
	 * @brief Inserts a new trade request into the database
	 * 
	 * @param tradeRequest Reference to the TradeRequest object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts trade request into database only
	 * @note Does not update local trade requests map
	 */
	bool Insert(TradeRequest &tradeRequest);
	
	/**
	 * @brief Selects generated trade requests for a specific account, coin, and side
	 * 
	 * @param generatedTradeRequests Reference to list to store generated trade request IDs
	 * @param accountId_generator ID of the account that generated the requests
	 * @param coin_id ID of the coin
	 * @param side Buy or sell side
	 * @param tradeRequests Reference to map of trade requests
	 * @return bool True if selection was successful, false otherwise
	 * 
	 * @note Retrieves generated trade requests for the specified parameters
	 * @note Populates the generatedTradeRequests list with IDs
	 */
	bool Select_generated(list<int> &generatedTradeRequests, int accountId_generator, int coin_id, side_enum side, map<int, TradeRequest> &tradeRequests);
	
	/**
	 * @brief Updates a trade request and the trade requests map
	 * 
	 * @param tradeRequest TradeRequest object containing updated information
	 * @param tradeRequests Reference to map of trade requests to update
	 * @return bool True if update was successful, false otherwise
	 * 
	 * @note Updates trade request in database and local map
	 * @note Updates the tradeRequests map with modified trade request
	 */
	bool Update(TradeRequest tradeRequest, map<int, TradeRequest> &tradeRequests);
	
	/**
	 * @brief Rejects inactive take profit/stop loss trades
	 * 
	 * @param inactiveTradeId ID of the inactive trade
	 * @param tradRequestId_except ID of trade request to exclude from rejection
	 * @return bool True if rejection was successful, false otherwise
	 * 
	 * @note Rejects inactive TP/SL trades except for the specified trade request
	 * @note Used for cleanup of inactive TP/SL positions
	 */
	bool RejectInactiveTpSl(int inactiveTradeId, int tradRequestId_except);
	
	/**
	 * @brief Deletes a trade request and updates the trade requests map
	 * 
	 * @param tradeRequest_id ID of the trade request to delete
	 * @param tradeRequests Reference to map of trade requests to update
	 * @return bool True if deletion was successful, false otherwise
	 * 
	 * @note Deletes trade request from database and local map
	 * @note Removes the trade request from the tradeRequests map
	 */
	bool Delete(int tradeRequest_id, map<int, TradeRequest> &tradeRequests);
	
	/**
	 * @brief Activates dependent trade requests based on obtained USDT
	 * 
	 * @param tradeRequest_id ID of the trade request that triggered activation
	 * @param obtainedUsdt Amount of USDT obtained from the trade
	 * 
	 * @note Activates dependent trade requests when sufficient USDT is obtained
	 * @note Used for cascading trade request activation
	 */
	void ActiveDependentTradeRequests(int tradeRequest_id, double obtainedUsdt);
	
	/**
	 * @brief Modifies quantity for active take profit/stop loss trades
	 * 
	 * @param tradeTpSl TradeTpSl object containing updated quantity information
	 * @return bool True if modification was successful, false otherwise
	 * 
	 * @note Updates quantity for active TP/SL trades
	 * @note Used for TP/SL quantity management
	 */
	bool ModifyQtyActiveTpSl(TradeTpSl tradeTpSl);
	
	/**
	 * @brief Deactivates dependent trade requests
	 * 
	 * @param tradeRequest_id ID of the trade request that triggered deactivation
	 * 
	 * @note Deactivates dependent trade requests when parent trade request is cancelled
	 * @note Used for cascading trade request deactivation
	 */
	void DeactiveDependentTradeRequests(int tradeRequest_id);
};
