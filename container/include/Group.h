#pragma once

#include "stdafx.h"
#include "Enums.h"


/**
 * @struct Group
 * @brief Data structure representing a group for account organization
 * 
 * This structure contains group information used for organizing and
 * categorizing accounts in the trading system. Groups allow for
 * hierarchical organization and management of user accounts.
 * 
 * Key Features:
 * - Group identification and metadata
 * - Group name for display and organization
 * 
 * @note Simple data structure for group information
 * @note Used for account organization and categorization
 */
struct Group
{
   int group_id=0;///< Unique identifier for the group
   string name="";///< Name of the group for display and organization
};