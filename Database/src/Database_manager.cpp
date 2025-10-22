/**
 * @file Database_manager.cpp
 * @brief Implementation of the Database_manager class for database operations
 *
 * This file contains the implementation of the Database_manager class which provides
 * centralized database management including connection handling, table initialization,
 * and database operations coordination.
 */

#include "Database_manager.h"

DatabaseHandler Database_manager::databaseHandler;///< Global database handler instance
sql::Driver *Database_manager::driver;///< MySQL driver instance
sql::Connection *Database_manager::con;///< Database connection instance
Trade_dbt Database_manager::trade_dbt(NULL);///< Trade database table handler
TradeRequest_dbt Database_manager::tradeRequest_dbt(NULL);///< TradeRequest database table handler
Account_dbt Database_manager::account_dbt(NULL);///< Account database table handler
Change_dbt Database_manager::change_dbt(NULL);///< Change database table handler
Coin_dbt Database_manager::coin_dbt(NULL);///< Coin database table handler
Commission_dbt Database_manager::commission_dbt(NULL);///< Commission database table handler
MarketResponse_dbt Database_manager::marketResponse_dbt(NULL);///< MarketResponse database table handler
MarketResponseDetail_dbt Database_manager::marketResponseDetail_dbt(NULL);///< MarketResponseDetail database table handler
Wallet_dbt Database_manager::wallet_dbt(NULL);///< Wallet database table handler
WalletHistory_dbt Database_manager::walletHistory_dbt(NULL);///< WalletHistory database table handler
TradeRequestGenerator_dbt Database_manager::tradeRequestGenerator_dbt(NULL);///< TradeRequestGenerator database table handler
Market_dbt Database_manager::market_dbt(NULL);///< Market database table handler
Message_dbt Database_manager::message_dbt(NULL);///< Message database table handler
CommissionRate_dbt Database_manager::commissionRate_dbt(NULL);///< CommissionRate database table handler
StairCommission_dbt Database_manager::stairCommission_dbt(NULL);///< StairCommission database table handler
DepositWithdrawal_dbt Database_manager::depositWithdrawal_dbt(NULL);///< DepositWithdrawal database table handler
CoinMinBalance_alertMessageThreshold_dbt Database_manager::coinMinBalance_alertMessageThreshold_dbt(NULL);///< CoinMinBalance_alertMessageThreshold database table handler
Group_dbt Database_manager::group_dbt(NULL);///< Group database table handler
BuyLimitation_dbt Database_manager::buyLimitation_dbt(NULL);///< BuyLimitation database table handler
GroupCoinMarketCommission_dbt Database_manager::groupCoinMarketCommission_dbt(NULL);///< GroupCoinMarketCommission database table handler
TradeRequestRobot_dbt Database_manager::tradeRequestRobot_dbt(NULL);///< TradeRequestRobot database table handler
Prices_dbt Database_manager::prices_dbt(NULL);///< Prices database table handler
LastTradePriceGenerator_dbt Database_manager::lastTradePriceGenerator_dbt(NULL);///< LastTradePriceGenerator database table handler
CommissionRateRule_dbt Database_manager::commissionRateRule_dbt(NULL);///< CommissionRateRule database table handler

/**
 * @brief Closes the database connection and cleans up resources
 *
 * This method closes the database connection, cleans up the connection object,
 * and terminates the MySQL driver. It handles various exception types and logs
 * the operation status.
 *
 * @return bool True if database was closed successfully, false otherwise
 *
 * @note Closes database statement and connection
 * @note Deletes connection object and terminates driver
 * @note Logs success or failure of the operation
 * @note Handles SQL exceptions and other error types
 */
bool Database_manager::Close_Database()
{
	bool res = false;
	try
	{
		databaseHandler.DB->close();
		con->close();
		delete con;
		driver->threadEnd();
		Logger::logger.log_special(LogLocation, false, "Database %s disconnected Successfully", Configuration::db_name.c_str());
		res = true;
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, false, "Failed To Close Connection to DataBase %s : %s", Configuration::db_name.c_str(), e.what());
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
	return res;
}

/**
 * @brief Initializes the database manager and all database table handlers
 *
 * This method initializes the database manager by setting up the database handler,
 * establishing the database connection, and initializing all database table handlers.
 * It also sets up the connection lost callback for automatic reconnection.
 *
 * @return bool True if initialization was successful, false otherwise
 *
 * @note Sets up shared mutex for thread safety
 * @note Configures connection lost callback for automatic reconnection
 * @note Initializes all database table handlers with database handler
 * @note Handles various exception types during initialization
 * @note Must be called before any database operations
 */
bool Database_manager::Initialize()
{
	bool res = false;
	try
	{
		databaseHandler.mut_DB = new shared_mutex();
		databaseHandler.OnConnectionLost = [&]()
		{
			// unique_lock lock(*databaseHandler.mut_DB);
			Close_Database();
			Open_Database();
		};
		res = Open_Database();

		trade_dbt = Trade_dbt(&databaseHandler);
		tradeRequest_dbt = TradeRequest_dbt(&databaseHandler);
		account_dbt = Account_dbt(&databaseHandler);
		change_dbt = Change_dbt(&databaseHandler);
		coin_dbt = Coin_dbt(&databaseHandler);
		commission_dbt = Commission_dbt(&databaseHandler);
		marketResponse_dbt = MarketResponse_dbt(&databaseHandler);
		marketResponseDetail_dbt = MarketResponseDetail_dbt(&databaseHandler);
		wallet_dbt = Wallet_dbt(&databaseHandler);
		walletHistory_dbt = WalletHistory_dbt(&databaseHandler);
		tradeRequestGenerator_dbt = TradeRequestGenerator_dbt(&databaseHandler);
		market_dbt = Market_dbt(&databaseHandler);
		message_dbt = Message_dbt(&databaseHandler);
		commissionRate_dbt = CommissionRate_dbt(&databaseHandler);
		stairCommission_dbt = StairCommission_dbt(&databaseHandler);
		depositWithdrawal_dbt = DepositWithdrawal_dbt(&databaseHandler);
		coinMinBalance_alertMessageThreshold_dbt = CoinMinBalance_alertMessageThreshold_dbt(&databaseHandler);
		group_dbt = Group_dbt(&databaseHandler);
		buyLimitation_dbt = BuyLimitation_dbt(&databaseHandler);
		groupCoinMarketCommission_dbt = GroupCoinMarketCommission_dbt(&databaseHandler);
		tradeRequestRobot_dbt = TradeRequestRobot_dbt(&databaseHandler);
		prices_dbt = Prices_dbt(&databaseHandler);
		lastTradePriceGenerator_dbt = LastTradePriceGenerator_dbt(&databaseHandler);
		commissionRateRule_dbt = CommissionRateRule_dbt(&databaseHandler);

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
	return res;
}

/**
 * @brief Opens a connection to the MySQL database
 *
 * This method establishes a connection to the MySQL database using the configuration
 * parameters. It sets up connection properties including hostname, username, password,
 * schema, port, and connection timeout. It also creates a statement object for
 * database operations.
 *
 * @return bool True if database connection was established successfully, false otherwise
 *
 * @note Uses MySQL Connector/C++ driver
 * @note Configures connection properties from Configuration class
 * @note Sets connection timeout to 5 seconds
 * @note Enables multi-statement support
 * @note Creates statement object for database operations
 * @note Logs connection success or failure
 * @note Handles SQL exceptions and other error types
 */
bool Database_manager::Open_Database()
{
	bool res = false;
	try
	{
		// https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-connect-options.html
		driver = get_driver_instance();
		sql::ConnectOptionsMap connection_properties;

		connection_properties["hostName"] = Configuration::db_host;
		connection_properties["userName"] = Configuration::db_username;
		connection_properties["password"] = Configuration::db_password;
		connection_properties["schema"] = Configuration::db_name;
		connection_properties["port"] = Configuration::db_port;
		//connection_properties["OPT_RECONNECT"] = false;
		connection_properties["OPT_CONNECT_TIMEOUT"] = 5;
		connection_properties["CLIENT_MULTI_STATEMENTS"] = true;

		con = driver->connect(connection_properties);

		assert(con != 0);

		if (!con->isClosed()) // <-- exception thrown
		{
			databaseHandler.DB = con->createStatement();
			databaseHandler.db_name = Configuration::db_name;

			Logger::logger.log_special(LogLocation, false, "Successful connection to database %s", Configuration::db_name.c_str());
			res = true;
		}
		else
		{
			Logger::logger.log_special(LogLocation, true, "Connection to database %s has failed.", Configuration::db_name.c_str());
		}
	}
	catch (sql::SQLException &e)
	{
		Logger::logger.log_special(LogLocation, true, "Connection to database %s has failed.# ERR: %s,MySQL error code: %i, SQLState: %s", Configuration::db_name.c_str(), e.what(), e.getErrorCode(), e.getSQLState().c_str());
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

	return res;
}