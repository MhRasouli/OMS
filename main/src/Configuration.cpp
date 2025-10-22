/**
 * @file Configuration.cpp
 * @brief Implementation of the Configuration class for application settings
 *
 * This file contains the implementation of the Configuration class which manages
 * application-wide configuration settings including database connection parameters
 * and logging configuration.
 */

#include "stdafx.h"
#include "Configuration.h"
#include "Logger.h"
#include "Database_manager.h"

string  Configuration::db_host = "tcp://localhost";///< Database host connection string
string  Configuration::db_username = "root";///< Database username
string  Configuration::db_password = "Mh2951367!";///< Database password
//string  Configuration::db_username = "crypto_demo_user";
//string  Configuration::db_password = "owTOTI34Nw5N69D9TL62lgexcEFGs34Nw5N69D9TLF";
string  Configuration::db_name = "crypto_demo_exchange";///< Database name

int  Configuration::db_port = 3306;///< Database port number

/**
 * @brief Loads configuration settings from Config.ini file
 *
 * This method loads application configuration from the Config.ini file. If the file
 * doesn't exist, it creates a default configuration file with current settings.
 * The configuration includes database connection parameters and logging verbosity.
 *
 * Configuration parameters loaded:
 * - database_host: Database server hostname
 * - database_port: Database server port
 * - database_name: Database name
 * - database_username: Database username
 * - database_password: Database password
 * - Log_verbose: Logging verbosity level (0 or 1)
 *
 * @note Creates default Config.ini file if it doesn't exist
 * @note Logs all loaded configuration parameters
 * @note Handles file I/O errors and exceptions
 * @note Exits application if configuration file cannot be created or opened
 */
void Configuration::LoadConfig()
{
	try
	{
		string line;
		std::vector<std::string> lineParts;
		ifstream inFile;
		string configFileName = "Config.ini";

		inFile.open(configFileName.c_str());
		if (!inFile) {
			std::ofstream outfile(configFileName.c_str());

			outfile << "database_host=" << db_host << std::endl;
			outfile << "database_port=" << db_port << std::endl;
			outfile << "database_name=" << db_name << std::endl;
			outfile << "database_username=" << db_username << std::endl;
			outfile << "database_password=" << db_password << std::endl;
			outfile << "Log_verbose=" << (Logger::logger.Verbose == true ? "1" : "0") << std::endl;
			

			outfile.close();

			//**********************************************************
			inFile.open(configFileName.c_str());
			if (!inFile) {
				Logger::logger.log_special(LogLocation, true, "Unable to open configuration file (Config.ini)");
				exit(1);
			}
		}

		while (std::getline(inFile, line))
		{
			boost::split(lineParts, line, boost::is_any_of("="));

			if (lineParts[0] == "database_host")
			{
				db_host = lineParts[1];
			}
			else if (lineParts[0] == "database_port")
			{
				db_port = stoi(lineParts[1]);
			}
			else if (lineParts[0] == "database_name")
			{
				db_name = lineParts[1];
			}
			else if (lineParts[0] == "database_username")
			{
				db_username = lineParts[1];
			}
			else if (lineParts[0] == "database_password")
			{
				db_password = lineParts[1];
			}
			else if (lineParts[0] == "Log_verbose")
			{
				if (lineParts[1] == "0")
				{
					Logger::logger.Verbose = false;
				}
				else
				{
					Logger::logger.Verbose = true;
				}
			}
		}

		inFile.close();

		Logger::logger.log_special(LogLocation, false, "<<< Cofigurations >>>");
		Logger::logger.log_special(LogLocation, false, "database_host=%s", db_host.c_str());
		Logger::logger.log_special(LogLocation, false, "database_port=%s", to_string(db_port).c_str());
		Logger::logger.log_special(LogLocation, false, "database_name=%s", db_name.c_str());
		Logger::logger.log_special(LogLocation, false, "database_username=%s", db_username.c_str());
		Logger::logger.log_special(LogLocation, false, "database_password=%s", db_password.c_str());
		Logger::logger.log_special(LogLocation, false, "Log_verbose=%s", (Logger::logger.Verbose == true ? "1" : "0"));
	}
	catch (std::exception ex)
	{
		Logger::logger.log_special(LogLocation, true, "Configuration initialize failed : %s", ex.what());
	}
	catch (...)
	{
		Logger::logger.log_special(LogLocation, true, "Configuration initialize failed : Unknown");
	}
}

