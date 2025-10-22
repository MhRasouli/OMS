#pragma once


#include "stdafx.h"

/**
 * @class Configuration
 * @brief Configuration management class for the trading service
 * 
 * This class manages application configuration settings, particularly database
 * connection parameters. It provides static methods and members for loading
 * and accessing configuration values throughout the application.
 * 
 * Features:
 * - Database connection configuration
 * - Static configuration loading
 * - Centralized configuration management
 * - Database connection parameters (host, username, password, database name, port)
 * 
 * The configuration is loaded once at application startup and provides
 * database connection details for the entire trading service.
 * 
 * @note All members are static for global access
 * @note Configuration is loaded at application startup
 * @note Manages database connection parameters
 */
class Configuration
{
public:
	static string db_host;///< Database server hostname or IP address
	static string db_username;///< Database username for authentication
	static string db_password;///< Database password for authentication
	static string db_name;///< Database name to connect to
	static int db_port;///< Database server port number

	/**
	 * @brief Loads configuration settings from external source
	 * 
	 * This method loads configuration parameters, typically from a configuration
	 * file or environment variables. It initializes the static database
	 * connection parameters used throughout the application.
	 * 
	 * @note Must be called at application startup before database operations
	 * @note Sets all static database connection parameters
	 */
	static void LoadConfig();
};