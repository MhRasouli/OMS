#pragma once

#include "stdafx.h"
#include "Enums.h"
#include "GroupCoinMarketCommission.h"
#include "Enums.h"
#include "CommissionRate.h"

/**
 * @struct TradeRequest
 * @brief Core data structure representing a trading request in the OMS system
 * 
 * This structure encapsulates all information related to a trading request including
 * order details, market data, commission calculations, and execution status.
 * It serves as the primary data model for trade processing throughout the system.
 * 
 * Key Features:
 * - Order identification and metadata
 * - Market price tracking and validation
 * - Commission rate calculations
 * - Stop order handling and conversion
 * - Buy/sell limitation management
 * - Take profit/Stop loss (TP/SL) support
 * - Dual region data management (OMS and Database)
 * 
 * @note Contains both OMS region and Database region data for synchronization
 * @note Supports various order types including limit, stop, and market orders
 * @note Includes commission calculation methods for different trade locations
 */
struct TradeRequest
{
	time_t time = 0;///< Timestamp when the trade request was created
	int tradeRequest_id = 0;///< Unique identifier for the trade request
	int account_id = 0;///< ID of the account making the trade request
	int coin_id = 0;///< ID of the cryptocurrency being traded
	int dependTo_TR_id = 0;///< ID of dependent trade request (for linked orders)
	status_enum status;///< Current status of the trade request
	bool isLock = false;///< Flag indicating if the order is locked for processing
	string rejectReason = "";///< Reason for order rejection (if applicable)
	string coinPair = "";///< Trading pair symbol (e.g., "BTCUSDT")
	type_enum type = type_enum::Limit;///< Order type (Limit, Market, Stop, etc.)
	bool visible = true;///< Flag indicating if order is visible in order book
	map<int, double> map_buyerLimitation_NotAccepted; ///< Map of buyer limitations not accepted [tr_seller.tradeRequest_id] = buyLimitation.max_buy_qty
	bool stopOrder2MarketOrder_Converted = false;///< Flag indicating if stop order was converted to market order

	bool stopOrder_isMoreThanMarketPrice = false;///< Flag for stop orders above market price
	bool stopOrder_isLessThanMarketPrice = false;///< Flag for stop orders below market price

	bool mustDelete = false;///< Flag indicating if the order should be deleted

	double if_sell_obtainedUsdt = 0;///< USDT amount that would be obtained if selling
	double if_sell_lostCoin = 0;///< Coin amount that would be lost if selling

	double if_buy_obtainedCoin = 0;///< Coin amount that would be obtained if buying
	double if_buy_lostUsdt = 0;///< USDT amount that would be lost if buying

	int tpSl_type = 0;///< Take profit/Stop loss type
	int tpsl_RefTrade_id = 0;///< Reference trade ID for TP/SL orders

	double coin_market_buy_price = 0;  ///< Current market bid price
	double coin_market_sell_price = 0; ///< Current market ask price
	time_t coin_market_time_price = 0;///< Timestamp of the market price data

	int buyLimitation_id = 0;///< ID of the buy limitation rule applied
	int locationInGeneratedQueue = 0;///< Position in the generated trading queue

	int reference_tradeRequest_id = 0;///< Reference to another trade request

	/**
	 * @brief Gets the commission rate for a specific trade location
	 * 
	 * @param commissionRate Reference to store the calculated commission rate
	 * @param tradeLocation Location where the trade will be executed
	 * @return bool True if commission rate was successfully calculated
	 */
	bool Get_CommissionRate(CommissionRate &commissionRate, tradeLocation_enum tradeLocation);
	
	/**
	 * @brief Checks if there is a commission rule for the specified trade location
	 * 
	 * @param tradeLocation Location where the trade will be executed
	 * @return bool True if a commission rule exists for the location
	 */
	bool Check_hasCommissionRule(tradeLocation_enum tradeLocation);

	/**
	 * @brief Locks the order for processing
	 * 
	 * @return bool True if the order was successfully locked
	 */
	bool Lock_order();

	/**
	 * @brief Sets new quantity in OMS region
	 * 
	 * @param quantity_OmsRegion New quantity value
	 * @param price_OmsRegion Price for quantity calculation (default 0)
	 */
	void set_new_quantity_OmsRegion(double quantity_OmsRegion, double price_OmsRegion= 0);
	
	/**
	 * @brief Sets new price in OMS region
	 * 
	 * @param price_OmsRegion New price value
	 */
	void set_new_price_OmsRegion(double price_OmsRegion);

	/**
	 * @brief Sets quantity in Database region
	 * 
	 * @param quantity_DbRegion Quantity value for database
	 */
	void set_quantity_DbRegion(double quantity_DbRegion);
	
	/**
	 * @brief Sets price in Database region
	 * 
	 * @param price Price value for database
	 */
	void set_price_DbRegion(double price);
	
	/**
	 * @brief Sets side in Database region
	 * 
	 * @param side Buy or sell side
	 */
	void set_side_DbRegion(side_enum side);

	/**
	 * @brief Gets quantity from OMS region
	 * 
	 * @param price_OmsRegion Price for quantity calculation (default 0)
	 * @return double Quantity value from OMS region
	 */
	double get_quantity_OmsRegion(double price_OmsRegion= 0);
	
	/**
	 * @brief Gets price from OMS region
	 * 
	 * @return double Price value from OMS region
	 */
	double get_price_OmsRegion();
	
	/**
	 * @brief Gets side from OMS region
	 * 
	 * @return side_enum Buy or sell side from OMS region
	 */
	side_enum get_side_OmsRegion();

	/**
	 * @brief Gets quantity from Database region
	 * 
	 * @return double Quantity value from Database region
	 */
	double get_quantity_DbRegion();
	
	/**
	 * @brief Gets price from Database region
	 * 
	 * @return double Price value from Database region
	 */
	double get_price_DbRegion();
	
	/**
	 * @brief Gets side from Database region
	 * 
	 * @return side_enum Buy or sell side from Database region
	 */
	side_enum get_side_DbRegion();

private:
	double m_quantity_DbRegion = 0;///< Private quantity value for Database region
	double m_price_DbRegion = 0;///< Private price value for Database region
	side_enum m_side_DbRegion;///< Private side value for Database region

	/**
	 * @brief Gets commission coefficient based on stair commission rules
	 * 
	 * @param account_id Account ID for commission calculation
	 * @param result Reference to store the calculated coefficient
	 * @return bool True if coefficient was successfully calculated
	 */
	bool Get_CommissionCoeff_BasedOnStair(int account_id, double &result);
	
	/**
	 * @brief Calculates commission rate for buyer
	 * 
	 * @param account_id Account ID for commission calculation
	 * @param CR Base commission rate
	 * @param commission_perc Commission percentage
	 * @return CommissionRate Calculated commission rate for buyer
	 */
	CommissionRate calc_buyer_commissionRate(int account_id, CommissionRate CR, double commission_perc);
	
	/**
	 * @brief Calculates commission rate for seller
	 * 
	 * @param account_id Account ID for commission calculation
	 * @param CR Base commission rate
	 * @return CommissionRate Calculated commission rate for seller
	 */
	CommissionRate calc_seller_commissionRate(int account_id, CommissionRate CR);
};
