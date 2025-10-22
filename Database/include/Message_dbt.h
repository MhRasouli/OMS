#pragma once

#include "stdafx.h"
#include "DatabaseTable.h"
#include "UtilityTools.h"
#include "Message.h"

/**
 * @class Message_dbt
 * @brief Database table class for Message entity operations
 * 
 * This class provides database operations for the Message entity including
 * insertion operations for system messages and notifications. It extends
 * DatabaseTable to provide specific functionality for message-related
 * database operations.
 * 
 * Features:
 * - Message insertion operations
 * - System notification storage
 * - Message tracking and management
 * 
 * @note Inherits from DatabaseTable for common database operations
 * @note Provides message-specific database functionality
 * @note Supports system message and notification storage
 */
class Message_dbt :public DatabaseTable
{
public:
	/**
	 * @brief Constructor for Message_dbt
	 * 
	 * @param databaseHandler Pointer to the database handler for connection management
	 */
	Message_dbt(DatabaseHandler* databaseHandler);
	
	/**
	 * @brief Inserts a new message record
	 * 
	 * @param message Reference to the Message object to insert
	 * @return bool True if insertion was successful, false otherwise
	 * 
	 * @note Inserts system message or notification into the database
	 * @note Stores message content, timestamps, and associations
	 * @note Returns false if insertion fails
	 */
	bool Insert(Message& message);
};