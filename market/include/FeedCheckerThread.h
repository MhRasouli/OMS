/**
 * @file FeedCheckerThread.h
 * @brief Thread for monitoring price feed data freshness
 *
 * This file contains the FeedCheckerThread class which monitors price feed
 * data to ensure it remains fresh and active.
 */

#pragma once

#include "AThread.h"
#include "Logger.h"
#include "Coin.h"

/**
 * @class FeedCheckerThread
 * @brief Thread for monitoring price feed data freshness
 *
 * This class extends AThread to provide monitoring functionality for price
 * feed data. It checks if the price feed is still providing fresh data
 * and alerts when the feed appears to have stopped.
 *
 * Features:
 * - Price feed data freshness monitoring
 * - Automatic feed status checking
 * - Alert system for stopped feeds
 * - Thread-based continuous monitoring
 *
 * @note Inherits from AThread for thread management
 * @note Monitors data freshness through timestamps
 * @note Provides alerts when feeds become stale
 */
class FeedCheckerThread :public AThread
{
	/**
	 * @brief Main thread function for monitoring price feed data
	 *
	 * @return unsigned Thread exit code
	 *
	 * @note Continuously monitors data freshness
	 * @note Checks timestamps to detect stale data
	 * @note Alerts when feed appears to have stopped
	 */
	unsigned ThreadFunction();
	
	time_t time_lastGottenData;///< Timestamp of the last received data
	/**
	 * @brief Alerts that the price feed has stopped
	 *
	 * @note Logs feed stop event
	 * @note May trigger feed restart procedures
	 */
	void voice_FeedIsStopped();
	bool* dataIsNew;///< Pointer to flag indicating if data is fresh

	//string symbolName;
	//int coin_id;
    //int maxLimitFeedStopped_sec;

	bool* wsb_mustDelete;///< Pointer to flag indicating if WebSocket should be deleted
	Coin* coin_ptr;///< Pointer to the Coin object being monitored

public:
	/**
	 * @brief Constructor for FeedCheckerThread
	 *
	 * @param DataIsNew Pointer to flag indicating if data is fresh
	 * @param Coin_ptr Pointer to the Coin object to monitor
	 * @param Wsb_mustDelete Pointer to flag indicating if WebSocket should be deleted
	 *
	 * @note Initializes monitoring parameters
	 * @note Sets up thread for continuous monitoring
	 */
	FeedCheckerThread(bool* DataIsNew, Coin* Coin_ptr, bool* Wsb_mustDelete);
};
