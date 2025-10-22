/**
 * @file MarketObjects.h
 * @brief Container structure for market-related objects
 *
 * This file contains the MarketObjects struct which serves as a container
 * for various market-related components and provides memory management.
 */

#pragma once

#include "stdafx.h"
#include "MarketWalletsSupervisor.h"
#include "PythonInterface.h"
#include "DepositWithdrawalProcessor.h"

class MarketWalletsSupervisor;///< Forward declaration for MarketWalletsSupervisor
class PythonInterface;///< Forward declaration for PythonInterface
class DepositWithdrawalProcessor;///< Forward declaration for DepositWithdrawalProcessor

/**
 * @struct MarketObjects
 * @brief Container structure for market-related objects
 *
 * This struct serves as a centralized container for various market-related
 * components including Python interface, wallet supervision, and deposit/withdrawal
 * processing. It provides a convenient way to manage and access these components
 * while ensuring proper memory management.
 *
 * Features:
 * - Centralized market component management
 * - Memory management for market objects
 * - Component lifecycle management
 * - Resource cleanup
 *
 * @note Contains pointers to market-related components
 * @note Provides memory management functionality
 * @note Serves as a container for market operations
 */
struct MarketObjects
{
    PythonInterface *pythonInterface = nullptr;///< Pointer to Python interface component
    MarketWalletsSupervisor *marketWalletsSupervisor = nullptr;///< Pointer to market wallets supervisor
    DepositWithdrawalProcessor *DW_Processor = nullptr;///< Pointer to deposit/withdrawal processor

    /**
     * @brief Frees memory allocated for market objects
     *
     * @note Deallocates all contained objects
     * @note Sets pointers to nullptr
     * @note Ensures proper cleanup of resources
     */
    void FreeMemory();
};
