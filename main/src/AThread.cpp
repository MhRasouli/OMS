#include "AThread.h"

/**
 * @brief Default constructor for AThread
 * 
 * Initializes the thread object with default state values.
 * All boolean flags are set to false and the thread pointer is uninitialized.
 */
AThread::AThread()
{
}

/**
 * @brief Virtual destructor for AThread
 * 
 * Ensures proper cleanup by stopping the thread and deleting the thread object.
 * All exceptions during cleanup are caught and logged to prevent program termination.
 * 
 * @note Handles runtime_error, std::exception, and unknown exceptions
 * @note Calls Stop() to gracefully terminate the thread before deletion
 */
AThread::~AThread()
{
	try
	{
		Stop();
		delete m_thread;
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special_if_verb(LogLocation, true, "The exception is thrown in AThread destructor");
	}
}

/**
 * @brief Stops the thread gracefully
 * 
 * Sets the atomic stop flag and waits for the thread to finish execution.
 * Uses thread::join() to ensure the thread completes before returning.
 * All exceptions during stopping are caught and logged.
 * 
 * @note Only attempts to stop if not already stopped
 * @note Includes a small sleep to allow thread to see the stop signal
 * @note Checks if thread is joinable before attempting to join
 */
void AThread::Stop()
{
	try
	{
		if (!stopped)
		{
			stopped = true;
			this_thread::sleep_for(chrono::milliseconds(1));

			if (m_thread->joinable())
				m_thread->join();
		}
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special_if_verb(LogLocation, true, "The exception is thrown in AThread Stop");
	}
}

/**
 * @brief Starts the thread with the given log message
 * 
 * Initializes thread state and creates a new std::thread that will execute ThreadStarter().
 * Can only be called once per thread instance to prevent multiple thread creation.
 * All exceptions during thread creation are caught and logged.
 * 
 * @param LogMessage String identifier for logging purposes
 * @return bool Always returns true (legacy return value)
 * 
 * @note Can only be called once per thread instance (PreStarted check)
 * @note Creates a new std::thread pointing to ThreadStarter method
 * @note Resets thread state flags before starting
 */
bool AThread::Start(std::string LogMessage)
{
	try
	{
		if (!PreStarted)
		{
			PreStarted = true;
			started = false;
			finished = false;

			logMessage = LogMessage;
			m_thread = new thread(&AThread::ThreadStarter, this);
		}
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}
	return true;
}

/**
 * @brief Internal method that starts the thread execution
 * 
 * This method is called by the std::thread and manages the thread lifecycle.
 * It logs thread start/finish, sets appropriate state flags, and calls the
 * virtual ThreadFunction() implemented by derived classes.
 * All exceptions during thread execution are caught and logged.
 * 
 * @note Logs thread start and finish messages
 * @note Manages thread state flags (PreStarted, started, finished)
 * @note Calls the virtual ThreadFunction() implemented by derived classes
 * @note Ensures proper cleanup of state flags even on exceptions
 */
void AThread::ThreadStarter()
{
	try
	{
		Logger::logger.log_special(LogLocation, false, "Thread %s started", logMessage.c_str());

		PreStarted = true;
		started = true;
		finished = false;

		ThreadFunction();

		finished = true;
		started = false;
		PreStarted = false;
		Logger::logger.log_special(LogLocation, false, "Thread %s finished", logMessage.c_str());
	}
	catch (const std::runtime_error &re)
	{
		// speciffic handling for runtime_error
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());
	}
	catch (const std::exception &ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption");
	}
}

/**
 * @brief Checks if the stop event has been signaled
 * 
 * Atomically loads the stopped flag and returns its value.
 * Includes a small sleep to prevent busy waiting in calling threads.
 * 
 * @return bool True if the thread should stop execution
 * 
 * @note Uses atomic load operation for thread safety
 * @note Includes 1ms sleep to prevent CPU spinning
 */
bool AThread::isStopEventSignaled() const
{
	this_thread::sleep_for(chrono::milliseconds(1));
	return stopped.load();
}

/**
 * @brief Checks if the thread is currently running
 * 
 * Determines if the thread is in an active running state by checking
 * that it has been pre-started and has not finished execution.
 * 
 * @return bool True if thread is in running state (PreStarted && !finished)
 */
bool AThread::isRunning()
{
	return PreStarted && !finished;
}

/**
 * @brief Checks if the thread has been started
 * 
 * Returns whether the thread has been initiated (PreStarted state).
 * This indicates the thread creation process has begun.
 * 
 * @return bool True if thread is in PreStarted state
 */
bool AThread::isStarted()
{
	return PreStarted;
}
