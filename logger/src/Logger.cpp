/**
 * @file Logger.cpp
 * @brief Implementation of the Logger singleton class for the trading service
 * 
 * This file contains the implementation of the Logger class which provides
 * comprehensive logging functionality with thread safety, file output,
 * and configurable verbosity levels.
 */

#include "stdafx.h"
#include "Logger.h"
#include "GlobalVariables.h"

Logger Logger::logger;///< Singleton instance of the logger

/**
 * @brief Default constructor for Logger
 * 
 * Initializes the logger with default values and sets ClassInitialized to false.
 * The logger must be initialized with Initialize() before use.
 */
Logger::Logger()
{
	ClassInitialized = false;
}

/**
 * @brief Replaces all occurrences of a substring in a string
 * 
 * @param subject Reference to the string to modify
 * @param search String to search for
 * @param replace String to replace with
 * 
 * @note Modifies the subject string in place
 * @note Continues replacing until no more occurrences are found
 */
void Logger::ReplaceStringInPlace(std::string &subject, const std::string &search, const std::string &replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

/**
 * @brief Internal logging method for writing log messages
 * 
 * @param location Source location string (typically from LogLocation macro)
 * @param IsError Whether this is an error message
 * @param message Log message content
 * 
 * @note Only logs if ClassInitialized is true
 * @note Removes newlines from message content
 * @note Formats timestamp and writes to daily log files
 * @note Includes location information if Verbose mode is enabled or if it's an error
 * @note Prints to console in non-service mode
 */
void Logger::LOG(string location, bool IsError, string message)
{
	if (!ClassInitialized)
		return;

	ReplaceStringInPlace(message, "\n", "");

	time_t now = std::time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *std::localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	std::string str_Log = "(";
	str_Log += buf;
	str_Log += ") ";
	/*
	str_Log += ")(Memory:";
	str_Log += std::to_string(TotalAllocatioted); // getPeakRSS());

	str_Log += " B)";

	int vm_usage = 0;
	int resident_set = 0;
	int num_threads = 0;
	// Get_mem_usage(vm_usage, resident_set, num_threads);

	str_Log += "(vm_usage:";
	str_Log += std::to_string(vm_usage);
	str_Log += " B)";

	str_Log += "(resident_set:";
	str_Log += std::to_string(resident_set);
	str_Log += " B)(";

	str_Log += std::to_string(num_threads);

	str_Log += " ThCnt)(";

	// std::ostringstream ss;
	// ss << std::this_thread::get_id();
	// str_Log += ss.str();

	str_Log += " ThId) ";
*/
	if (IsError)
		str_Log += "Error : ";

	char str_message[1024];

	if (message.length() > 1024)
		message = message.substr(0, 1024);

	sprintf(str_message, "%-50s", message.c_str());
	str_Log += str_message;

	if (Verbose || IsError) //&& message.size() < 50)
	{
		str_Log += "(";
		str_Log += location;
		str_Log += ")";
	}

	str_Log += "\n";

	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

	std::string LogFolderPath = logFolderPath;
	LogFolderPath += buf;
	LogFolderPath += ".log";

	std::ofstream LogFile;
	LogFile.open(LogFolderPath.c_str(), std::ios::out | std::ios::app);
	LogFile << str_Log.c_str();
	LogFile.close();
#ifndef ServiceMode
	std::printf(str_Log.c_str());
#endif
}

/**
 * @brief Internal verbose logging method
 * 
 * @param location Source location string (typically from LogLocation macro)
 * @param IsError Whether this is an error message
 * @param message Log message content
 * 
 * @note Only logs if ClassInitialized is true and Verbose mode is enabled
 * @note Calls LOG() method if verbose logging is enabled
 */
void Logger::LOG_IF_VERB(string location, bool IsError, string message)
{
	if (!ClassInitialized)
		return;

	if (Verbose)
	{
		LOG(location, IsError, message);
	}
}

/**
 * @brief Logs a formatted message with variadic arguments
 * 
 * @param location Source location string (typically from LogLocation macro)
 * @param IsError Whether this is an error message
 * @param pMsg Format string (printf-style)
 * @param ... Variadic arguments for formatting
 * 
 * @note Uses printf-style formatting
 * @note Thread-safe operation with unique_lock
 * @note Only logs if ClassInitialized is true
 * @note Formats arguments using vsnprintf and calls LOG()
 */
void Logger::log_special(string location, bool IsError, const char *pMsg, ...)
{
	if (!ClassInitialized)
		return;

	unique_lock lock(extSync);

	/** Handling the var list. */
	char buffer[4096];
	va_list arg;
	va_start(arg, pMsg);
	vsnprintf(buffer, 4096, pMsg, arg);
	va_end(arg);
	/** Calling the internal log on the resulted char array. */
	LOG(location, IsError, buffer);
}

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
 * @note Thread-safe operation with unique_lock
 * @note Only logs if ClassInitialized is true
 * @note Formats arguments using vsnprintf and calls LOG()
 */
void Logger::log_special_if_verb(string location, bool IsError, const char *pMsg, ...)
{
	if (!ClassInitialized)
		return;

	unique_lock lock(extSync);

	/** Checking if the VERBOSE mode is on. */
	if (!Verbose)
	{
		return;
	}
	/** Handling the var list. */
	char buffer[4096];
	va_list arg;
	va_start(arg, pMsg);
	vsnprintf(buffer, 4096, pMsg, arg);
	va_end(arg);
	/** Calling the internal log on the resulted char array. */
	LOG(location, IsError, buffer);
}

/**
 * @brief Initializes the logger with a log folder path
 * 
 * @param LogFolder Path to the directory where log files will be stored
 * 
 * @note Sets up the log file and enables logging functionality
 * @note Must be called before any logging operations
 * @note Sets ClassInitialized to true to enable logging
 */
void Logger::Initialize(std::string LogFolder)
{
	Logger::logFolderPath = LogFolder;
	Logger::ClassInitialized = true;
}

/**
 * @brief Gets memory usage statistics (unused)
 * 
 * @param vm_usage Reference to store virtual memory usage
 * @param resident_set Reference to store resident set size
 * @param num_threads Reference to store thread count
 * 
 * @note Currently unused method for memory usage tracking
 * @note Reads from /proc/self/stat on Linux systems
 * @note Calculates virtual memory and resident set size
 */
void Logger::Get_mem_usage(int &vm_usage, int &resident_set, int &num_threads)
{
	vm_usage = 0;
	resident_set = 0;

	// the two fields we want
	unsigned long vsize;
	long rss;

	{
		std::string ignore;
		std::ifstream ifs("/proc/self/stat", std::ios_base::in);
		ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> num_threads >> ignore >> ignore >> vsize >> rss;
	}

	long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	vm_usage = vsize / 1024;
	resident_set = rss * page_size_kb;
}

/**
 * @brief Gets the current thread count (unused)
 * 
 * @return int Current thread count
 * 
 * @note Currently unused method for thread count tracking
 * @note Contains commented Windows-specific implementation
 * @note Returns 0 as placeholder
 */
int Logger::GetCurrentThreadCount()
{
	/*
	// first determine the id of the current process
	DWORD const  id = GetCurrentProcessId();

	// then get a process list snapshot.
	HANDLE const  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	// initialize the process entry structure.
	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);

	// get the first process info.
	BOOL  ret = true;
	ret = Process32First(snapshot, &entry);
	while (ret && entry.th32ProcessID != id) {
		ret = Process32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
	return ret
		? entry.cntThreads
		: -1;
	*/
	return 0;
}
