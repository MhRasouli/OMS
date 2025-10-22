#pragma once

#include "stdafx.h"
#include "Enums.h"

/**
 * @struct DepositWithdrawal
 * @brief Data structure representing deposit and withdrawal operations
 * 
 * This structure contains all information related to deposit and withdrawal
 * operations including request details, execution status, fees, and timing.
 * It serves as the data model for managing cryptocurrency transfers.
 * 
 * Key Features:
 * - Deposit/withdrawal identification and metadata
 * - Account and coin associations
 * - Quantity tracking (requested, commission, final)
 * - Operation type and status management
 * - Timing information (request and execution)
 * - Network fee tracking
 * - Processing status and rejection handling
 * 
 * @note Contains comprehensive deposit/withdrawal operation data
 * @note Tracks both requested and final quantities
 * @note Includes commission and network fee information
 * @note Supports rejection reason tracking
 */
struct DepositWithdrawal
{
    int depositWithdrawal_id = 0;///< Unique identifier for the deposit/withdrawal operation
    int account_id = 0;///< ID of the account performing the operation
    int coin_id = 0;///< ID of the cryptocurrency being deposited/withdrawn
    double quantity_request = 0;///< Original quantity requested for the operation
    double commission_quantity = 0;///< Commission quantity deducted from the operation
    type_DW_enum type;///< Type of operation (deposit or withdrawal)
    status_enum status;///< Current status of the operation
    double quantity_final = 0;///< Final quantity after processing (including fees)
    time_t time_execution = 0;///< Timestamp when the operation was executed
    int reject_reason_id = 0;///< ID of the rejection reason (if operation was rejected)
    bool is_node;///< Flag indicating if this is a node operation
    time_t time_request = 0;///< Timestamp when the operation was requested
    double network_fee_qty=0;///< Network fee quantity for the operation

    bool processed=false;///< Flag indicating if the operation has been processed
};