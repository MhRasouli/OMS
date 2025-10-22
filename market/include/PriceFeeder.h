#pragma once
#include "stdafx.h"
#include "AThread.h"

/**
 * @class PriceFeeder
 * @brief Abstract base class for price data feeding in the trading service
 * 
 * This class serves as the base class for all price feeder implementations
 * that retrieve real-time market price data from various cryptocurrency exchanges.
 * It extends AThread to provide threaded execution for continuous price monitoring.
 * 
 * Features:
 * - Threaded execution for continuous price monitoring
 * - Token renewal mechanism for API authentication
 * - Abstract data stopping method for clean shutdown
 * - Deletion flag for lifecycle management
 * 
 * Derived classes implement specific exchange APIs (Binance, Kucoin, Nobitex, etc.)
 * and provide real-time price data to the trading system.
 * 
 * @note This is an abstract class - derived classes must implement StopGetData()
 * @note Inherits thread management capabilities from AThread
 * @note Supports token-based authentication renewal
 * @note Used by PriceFeederManager for coordinated price feeding
 */
class PriceFeeder : public AThread
{
public:
	/**
	 * @brief Default constructor for PriceFeeder
	 * 
	 * Initializes the price feeder with default values and sets mustDelete to false.
	 */
	PriceFeeder();
	
	/**
	 * @brief Virtual method for renewing authentication tokens
	 * 
	 * This method should be implemented by derived classes to handle
	 * token renewal for API authentication. Called periodically to
	 * maintain valid API access.
	 * 
	 * @note Default implementation is empty - derived classes should override
	 * @note Called by the price feeding thread to maintain API access
	 */
	virtual void RenewToken();
	
	/**
	 * @brief Pure virtual method to stop data retrieval
	 * 
	 * This method must be implemented by derived classes to properly
	 * stop the price data retrieval process. It should clean up
	 * connections, cancel ongoing requests, and prepare for shutdown.
	 * 
	 * @note Pure virtual - must be implemented by derived classes
	 * @note Called during thread shutdown to ensure clean termination
	 */
	virtual void StopGetData()=0;
	
	bool mustDelete;///< Flag indicating if this price feeder should be deleted
};