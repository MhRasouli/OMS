#pragma once

/**
 * @enum Enum_QueueCellType
 * @brief Enumeration for queue cell operation types
 */
enum Enum_QueueCellType
{
	trade_add,///< Add a new trade to the queue
	trade_update///< Update an existing trade in the queue
};

/**
 * @enum side_enum
 * @brief Enumeration for trade side (buy or sell)
 */
enum class side_enum
{
	Buy,///< Buy side of the trade
	Sell///< Sell side of the trade
};

/**
 * @enum changeType_enum
 * @brief Enumeration for database change operation types
 */
enum class changeType_enum
{
	Update,///< Update an existing record
	Insert,///< Insert a new record
	Delete///< Delete an existing record
};

/**
 * @enum status_enum
 * @brief Enumeration for trade request status
 */
enum class status_enum
{
	Active = 0,///< Trade request is active and ready for execution
	Confirm = 1,///< Trade request is confirmed
	Cancel = 2,///< Trade request is cancelled
	Rejected = 3///< Trade request is rejected
};

/**
 * @enum type_enum
 * @brief Enumeration for order types
 */
enum class type_enum
{
	None = -1,///< No order type specified
	Market = 0,///< Market order (execute at current market price)
	Limit = 1,///< Limit order (execute at specified price or better)
	Stop = 2,///< Stop order (triggered when price reaches stop level)
	Swap = 3///< Swap order (for position swapping)
};

/**
 * @enum type_DW_enum
 * @brief Enumeration for deposit/withdrawal types
 */
enum class type_DW_enum
{
	Deposit = 0,///< Deposit operation
	Withdrawal = 1///< Withdrawal operation
};

/**
 * @enum rejectReason_DW_enum
 * @brief Enumeration for deposit/withdrawal rejection reasons
 */
enum class rejectReason_DW_enum
{
	None = 0,///< No rejection reason
	HasActiveMarketOrder = 1,///< Account has active market orders
	TotalLockOrderQty = 2,///< Total locked order quantity exceeds limit
	AccountBalanceIsNotEnough = 3,///< Account balance is insufficient
	CoinIsNotExist = 4///< Coin does not exist in the system
};

/**
 * @enum tradeLocation_enum
 * @brief Enumeration for trade execution locations
 */
enum class tradeLocation_enum
{
	Market = 0,///< Trade executed on external market/exchange
	Broker = 1,///< Trade executed through broker
	Local = 2///< Trade executed locally within the system
};

/**
 * @enum robotType_enum
 * @brief Enumeration for trading robot types
 */
enum class robotType_enum
{
	Seller=0,///< Seller robot type
	Buyer=1///< Buyer robot type
};