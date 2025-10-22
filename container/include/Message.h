#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct Message
 * @brief Data structure representing system messages and notifications
 * 
 * This structure contains message information for system notifications,
 * alerts, and user communications. It tracks message creation, confirmation,
 * and association with specific accounts and coins.
 * 
 * Key Features:
 * - Message identification and content
 * - Confirmation status tracking
 * - Creation and confirmation timestamps
 * - Account and coin associations
 * 
 * @note Contains system message and notification data
 * @note Tracks message lifecycle (creation, confirmation)
 * @note Supports account and coin-specific messaging
 */
struct Message
{
   int message_id=0;///< Unique identifier for the message
   string message="";///< Content of the message
   bool confirm=false;///< Flag indicating if the message has been confirmed
   time_t createTime=0;///< Timestamp when the message was created
   time_t confirmTime=0;///< Timestamp when the message was confirmed
   int coin_id=0;///< ID of the coin associated with the message
   int account_id=0;///< ID of the account associated with the message
};