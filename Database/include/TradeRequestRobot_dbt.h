/**
 * @file TradeRequestRobot_dbt.h
 * @brief Database table class for TradeRequestRobot entity operations
 *
 * This file contains the TradeRequestRobot_dbt class which provides
 * database operations for trade request robot configurations.
 */

#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "TradeRequestRobot.h"

/**
 * @class TradeRequestRobot_dbt
 * @brief Database table class for TradeRequestRobot entity operations
 *
 * This class provides database operations for the TradeRequestRobot entity
 * including selection operations for automated trading robot configurations.
 * It extends DatabaseTable to provide specific functionality for trade request
 * robot database operations.
 *
 * Features:
 * - Trade request robot selection operations
 * - Individual and bulk robot selection
 * - Robot configuration management
 *
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides trade request robot-specific database functionality
 * @note Supports both individual and bulk robot operations
 */
class TradeRequestRobot_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for TradeRequestRobot_dbt
	 *
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	TradeRequestRobot_dbt(DatabaseHandler* databaseHandler);

	/**
	 * @brief Selects a specific trade request robot by ID
	 *
	 * @param tradeRequestRobots Reference to map to store the selected robot
	 * @param tradeRequestRobot_id ID of the trade request robot to select
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves specific trade request robot configuration from the database
	 * @note Populates the tradeRequestRobots map with the selected robot
	 * @note Returns false if robot not found
	 */
	bool Select(map<int, TradeRequestRobot>& tradeRequestRobots,int tradeRequestRobot_id);

	/**
	 * @brief Selects all trade request robots and populates the provided map
	 *
	 * @param tradeRequestRobots Reference to map to store all robots indexed by robot ID
	 * @return bool True if selection was successful, false otherwise
	 *
	 * @note Retrieves all trade request robot configurations from the database
	 * @note Populates the tradeRequestRobots map with robot ID as key
	 * @note Returns false if selection fails
	 */
	bool SelectAll(map<int, TradeRequestRobot>& tradeRequestRobots);
};