#pragma once
#include "stdafx.h"

#define LogLocation string(__FILE__).substr(string(__FILE__).find_last_of("/\\") + 1) + "->" + string(__PRETTY_FUNCTION__) + "->" + std::to_string(__LINE__)
//#define LogLocation  string(__FILE__).substr(string(__FILE__).find_last_of("/\\") + 1)+"->"+std::to_string(__LINE__)

/**
 * @class Logger
 * @brief Singleton logging system for the trading service
 * 
 * This class provides comprehensive logging functionality with thread safety,
 * file output, memory usage tracking, and configurable verbosity levels.
 * It serves as the central logging mechanism for the entire trading service.
 * 
 * Features:
 * - Thread-safe logging with shared_mutex
 * - File-based log output with timestamp formatting
 * - Memory usage tracking and reporting
 * - Configurable verbosity levels
 * - Variadic argument support for formatted logging
 * - Singleton pattern for global access
 * 
 * @note Thread-safe operations using shared_mutex
 * @note Singleton instance accessible via Logger::logger
 * @note Supports both regular and verbose logging modes
 */
class Logger
{
private:
	shared_mutex extSync;///< Shared mutex for thread-safe logging operations
	FILE *logFile;///< File pointer for log output
	bool ClassInitialized;///< Flag indicating if logger has been initialized
	std::string logFolderPath;///< Path to the log directory

	/**
	 * @brief Replaces all occurrences of a substring in a string
	 * 
	 * @param subject Reference to the string to modify
	 * @param search String to search for
	 * @param replace String to replace with
	 */
	void ReplaceStringInPlace(std::string &subject, const std::string &search, const std::string &replace);
	
	/**
	 * @brief Gets the current thread count (unused)
	 * 
	 * @return int Current thread count
	 */
	int GetCurrentThreadCount();
	
	/**
	 * @brief Gets memory usage statistics (unused)
	 * 
	 * @param vm_usage Reference to store virtual memory usage
	 * @param resident_set Reference to store resident set size
	 * @param num_threads Reference to store thread count
	 */
	void Get_mem_usage(int &vm_usage, int &resident_set, int &num_threads);

	/**
	 * @brief Internal logging method
	 * 
	 * @param location Source location string
	 * @param IsError Whether this is an error message
	 * @param message Log message content
	 */
	void LOG(string location, bool IsError, string message);
	
	/**
	 * @brief Internal verbose logging method
	 * 
	 * @param location Source location string
	 * @param IsError Whether this is an error message
	 * @param message Log message content
	 */
	void LOG_IF_VERB(string location, bool IsError, string message);

public:
	/**
	 * @brief Default constructor for Logger
	 * 
	 * Initializes the logger with default values and sets ClassInitialized to false.
	 */
	Logger();
	
	/**
	 * @brief Initializes the logger with a log folder path
	 * 
	 * Sets up the log file and enables logging functionality.
	 * Must be called before any logging operations.
	 * 
	 * @param LogFolder Path to the directory where log files will be stored
	 */
	void Initialize(std::string LogFolder);

	/**
	 * @brief Logs a formatted message with variadic arguments
	 * 
	 * @param location Source location string (typically from LogLocation macro)
	 * @param IsError Whether this is an error message
	 * @param pMsg Format string (printf-style)
	 * @param ... Variadic arguments for formatting
	 * 
	 * @note Uses printf-style formatting
	 * @note Thread-safe operation
	 */
	void log_special(string location, bool IsError, const char *pMsg, ...);
	
	/**
	 * @brief Logs a formatted message only if verbose mode is enabled
	 * 
	 * @param location Source location string (typically from LogLocation macro)
	 * @param IsError Whether this is an error message
	 * @param pMsg Format string (printf-style)
	 * @param ... Variadic arguments for formatting
	 * 
	 * @note Only logs if Verbose flag is true
	 * @note Uses printf-style formatting
	 * @note Thread-safe operation
	 */
	void log_special_if_verb(string location, bool IsError, const char *pMsg, ...);
	
	uint32_t TotalAllocatioted;///< Total allocated memory counter
	shared_mutex Mux_TotalAllocatioted;///< Mutex for TotalAllocatioted access
	bool Verbose;///< Flag controlling verbose logging output

	static Logger logger;///< Singleton instance of the logger
};
