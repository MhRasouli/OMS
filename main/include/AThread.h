#pragma once

#include "stdafx.h"
#include "Logger.h"

/**
 * @class AThread
 * @brief Abstract base class for thread management in the trading service
 * 
 * This class provides a framework for creating and managing threads with proper
 * lifecycle management, exception handling, and logging. It serves as the base
 * class for all threaded components in the trading service.
 * 
 * Features:
 * - Thread lifecycle management (start, stop, join)
 * - Exception handling with logging
 * - Thread state tracking (started, running, finished)
 * - Atomic stop signal for thread coordination
 * - Virtual thread function for derived classes to implement
 * 
 * @note This class is abstract and requires derived classes to implement ThreadFunction()
 * @note Copy constructor and assignment operator are deleted
 * @note All exceptions are caught and logged
 */
class AThread
{
private :
	bool started = false;///< Flag indicating if thread has started execution
	bool PreStarted = false;///< Flag indicating if thread is in pre-start state
	bool finished = false;///< Flag indicating if thread has finished execution
	
	std::string logMessage="";///< Log message identifier for this thread
	void ThreadStarter();///< Internal method that starts the thread execution

	thread* m_thread;///< Pointer to the underlying std::thread object

public:
	/**
	 * @brief Default constructor for AThread
	 * 
	 * Initializes the thread object with default state values.
	 */
	AThread();
	
	/**
	 * @brief Virtual destructor for AThread
	 * 
	 * Ensures proper cleanup by stopping the thread and deleting the thread object.
	 * All exceptions during cleanup are caught and logged.
	 */
	virtual ~AThread();
	
	/**
	 * @brief Starts the thread with the given log message
	 * 
	 * @param LogMessage String identifier for logging purposes
	 * @return bool Always returns true (legacy return value)
	 * 
	 * @note Can only be called once per thread instance
	 * @note Creates a new std::thread and starts ThreadStarter()
	 */
	bool Start(std::string LogMessage);
	
	/**
	 * @brief Checks if the thread is currently running
	 * 
	 * @return bool True if thread is in running state (PreStarted && !finished)
	 */
	bool isRunning();
	
	/**
	 * @brief Initializes the thread (currently unused)
	 */
	void Initialize();
	
	/**
	 * @brief Deleted copy constructor
	 * 
	 * Prevents copying of AThread objects to avoid thread management issues.
	 */
	AThread(const AThread&)=delete;
	
	/**
	 * @brief Deleted assignment operator
	 * 
	 * Prevents assignment of AThread objects to avoid thread management issues.
	 */
	AThread& operator=(const AThread&) = delete;
	
	/**
	 * @brief Checks if the thread has been started
	 * 
	 * @return bool True if thread is in PreStarted state
	 */
	bool isStarted();
	
	/**
	 * @brief Stops the thread gracefully
	 * 
	 * Sets the stop signal and waits for the thread to finish.
	 * All exceptions during stopping are caught and logged.
	 */
	void Stop();

protected:
	/**
	 * @brief Pure virtual function that derived classes must implement
	 * 
	 * This function contains the main logic that will run in the thread.
	 * Derived classes must implement this method to define their thread behavior.
	 * 
	 * @return unsigned Return value from thread execution (currently unused)
	 */
	virtual unsigned ThreadFunction() = 0;
	
	/**
	 * @brief Checks if the stop event has been signaled
	 * 
	 * @return bool True if the thread should stop execution
	 * 
	 * @note Includes a small sleep to prevent busy waiting
	 */
	bool isStopEventSignaled() const;
	
	std::atomic<bool> stopped = false;///< Atomic flag for thread stop coordination
};
