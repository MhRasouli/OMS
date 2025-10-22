#include "DepositWithdrawalProcessor.h"
#include "GlobalVariables.h"
#include "Enums.h"
#include "Database_manager.h"
#include "UtilityTools.h"
#include "TradeRequestProcessor.h"

DepositWithdrawalProcessor::DepositWithdrawalProcessor(Market _market) : market(_market)
{}

bool DepositWithdrawalProcessor::Update_depositWithdrawal_dbt(DepositWithdrawal &depoWithd_req, double final_qty, rejectReason_DW_enum rejectReason_id = rejectReason_DW_enum::None)
{
    bool result = false;
    if (depoWithd_req.type == type_DW_enum::Deposit || final_qty > 0)
    {
        depoWithd_req.quantity_final = final_qty;
        depoWithd_req.status = status_enum::Confirm;
    }
    else
    {
        depoWithd_req.quantity_final = 0;
        depoWithd_req.status = status_enum::Rejected;
    }
    depoWithd_req.time_execution = time(0);
    depoWithd_req.reject_reason_id = (int)rejectReason_id;
    result = Database_manager::depositWithdrawal_dbt.Update(depoWithd_req);

    return result;
}

bool DepositWithdrawalProcessor::Withdrawal(DepositWithdrawal &withdrawal_req)
{
    bool result = false;
    try
    {
        map<int, double> account_needs; // 0 : is USDT and 1,2,3,....  is coin id

        for (map<int, Coin>::iterator it_coin = GlobalVariables::coins.begin(); it_coin != GlobalVariables::coins.end(); it_coin++)
        {
            //unique_lock lock(it_coin->second.tradeRequestProcessor->mux);

            for (map<int, TradeRequest>::iterator it_trdReq = it_coin->second.tradeRequestProcessor->tradeRequests.begin();
                 it_trdReq != it_coin->second.tradeRequestProcessor->tradeRequests.end(); it_trdReq++)
            {
                if (it_trdReq->second.account_id == withdrawal_req.account_id)
                {
                    if (!it_trdReq->second.mustDelete && it_trdReq->second.status == status_enum::Active && it_trdReq->second.isLock)
                    {
                        if (it_trdReq->second.get_price_OmsRegion() > 0 && it_trdReq->second.type != type_enum::Market && !it_trdReq->second.stopOrder2MarketOrder_Converted)
                        {
                            if (it_trdReq->second.get_side_OmsRegion() == side_enum::Buy)
                            {
                                if (account_needs.find(0) == account_needs.end())
                                    account_needs[0] = 0;

                                CommissionRate commissionRate;
                                if (it_trdReq->second.Get_CommissionRate(commissionRate,tradeLocation_enum::Local))
                                {
                                    account_needs[0] += it_trdReq->second.get_quantity_OmsRegion() * it_trdReq->second.get_price_OmsRegion() * commissionRate.buyer_commission_perc;
                                }
                            }
                            else
                            {
                                if (account_needs.find(it_trdReq->second.coin_id) == account_needs.end())
                                    account_needs[it_trdReq->second.coin_id] = 0;

                                account_needs[it_trdReq->second.coin_id] += it_trdReq->second.get_quantity_OmsRegion();
                            }
                        }
                        else
                        {
                            return Update_depositWithdrawal_dbt(withdrawal_req, 0, rejectReason_DW_enum::HasActiveMarketOrder);
                        }
                    }
                }
            }
        }

        //**********************************
        WalletHistory walletHistory;

        walletHistory.trade_id = 0;
        walletHistory.time = time(0);
        walletHistory.depositWithdrawal_id = withdrawal_req.depositWithdrawal_id;
        if (withdrawal_req.coin_id == 0)
        {
            if (account_needs.find(0) != account_needs.end())
            {
                if ((account_needs[0] + withdrawal_req.quantity_request) > GlobalVariables::accounts[withdrawal_req.account_id].balance_usdt)
                {
                    return Update_depositWithdrawal_dbt(withdrawal_req, 0, rejectReason_DW_enum::TotalLockOrderQty);
                }
            }
            else if ((withdrawal_req.quantity_request) > GlobalVariables::accounts[withdrawal_req.account_id].balance_usdt)
            {
                return Update_depositWithdrawal_dbt(withdrawal_req, 0, rejectReason_DW_enum::AccountBalanceIsNotEnough);
            }

            walletHistory.account_id = withdrawal_req.account_id;
            walletHistory.coin_id = 0;
            walletHistory.balanceBefore_usd = GlobalVariables::accounts[withdrawal_req.account_id].balance_usdt;
            walletHistory.balanceAfter_usd = GlobalVariables::accounts[withdrawal_req.account_id].balance_usdt - withdrawal_req.quantity_request;

            if (Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_account_balance_usdt(walletHistory.account_id, walletHistory.balanceAfter_usd)))
            {
                if (Update_depositWithdrawal_dbt(withdrawal_req, withdrawal_req.quantity_request - withdrawal_req.commission_quantity))
                {
                    GlobalVariables::accounts[withdrawal_req.account_id].balance_usdt = walletHistory.balanceAfter_usd;
                    result = true;

                    if (!withdrawal_req.is_node)
                    {
                        walletHistory.account_id = market.accountID_TradeWith_TradeIsNotActive;
                        walletHistory.balanceBefore_usd = GlobalVariables::accounts[walletHistory.account_id].balance_usdt;
                        walletHistory.balanceAfter_usd = GlobalVariables::accounts[walletHistory.account_id].balance_usdt + withdrawal_req.quantity_request;

                        result = Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_account_balance_usdt(walletHistory.account_id, walletHistory.balanceAfter_usd));
                    }
                }
            }
        }
        else
        {
            double account_coin_walletQty = UtilityTools::Get_wallet_quantity(withdrawal_req.account_id, withdrawal_req.coin_id);
            if (account_needs.find(withdrawal_req.coin_id) != account_needs.end())
            {
                if ((account_needs[withdrawal_req.coin_id] + withdrawal_req.quantity_request) > account_coin_walletQty)
                {
                    return Update_depositWithdrawal_dbt(withdrawal_req, 0, rejectReason_DW_enum::TotalLockOrderQty);
                }
            }
            else if ((withdrawal_req.quantity_request) > account_coin_walletQty)
            {
                return Update_depositWithdrawal_dbt(withdrawal_req, 0, rejectReason_DW_enum::AccountBalanceIsNotEnough);
            }

            walletHistory.account_id = withdrawal_req.account_id;
            walletHistory.coin_id = withdrawal_req.coin_id;
            walletHistory.coinBefore_qty = account_coin_walletQty;
            walletHistory.coinAfter_qty = account_coin_walletQty - withdrawal_req.quantity_request;

            if (Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_wallet_quantity(walletHistory.account_id, walletHistory.coin_id, walletHistory.coinAfter_qty)))
            {
                if (Update_depositWithdrawal_dbt(withdrawal_req, withdrawal_req.quantity_request - withdrawal_req.commission_quantity))
                {
                    UtilityTools::Set_wallet_quantity(withdrawal_req.account_id, withdrawal_req.coin_id, walletHistory.coinAfter_qty);
                    result = true;

                    if (!withdrawal_req.is_node)
                    {
                        walletHistory.account_id = market.accountID_TradeWith_TradeIsNotActive;
                        double account_coin_walletQty = UtilityTools::Get_wallet_quantity(walletHistory.account_id, withdrawal_req.coin_id);
                        walletHistory.coin_id = withdrawal_req.coin_id;
                        walletHistory.coinBefore_qty = account_coin_walletQty;
                        walletHistory.coinAfter_qty = account_coin_walletQty + withdrawal_req.quantity_request;

                        result = Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_wallet_quantity(walletHistory.account_id, walletHistory.coin_id, walletHistory.coinAfter_qty));
                    }
                }
            }
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

    return result;
}

bool DepositWithdrawalProcessor::Deposit(DepositWithdrawal &deposit_req)
{
    bool result = false;
    try
    {
        WalletHistory walletHistory;

        walletHistory.trade_id = 0;
        walletHistory.time = time(0);
        walletHistory.depositWithdrawal_id = deposit_req.depositWithdrawal_id;
        if (deposit_req.coin_id == 0)
        {
            walletHistory.account_id = deposit_req.account_id;
            walletHistory.coin_id = 0;
            walletHistory.balanceBefore_usd = GlobalVariables::accounts[deposit_req.account_id].balance_usdt;
            walletHistory.balanceAfter_usd = GlobalVariables::accounts[deposit_req.account_id].balance_usdt + deposit_req.quantity_request;

            if (Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_account_balance_usdt(walletHistory.account_id, walletHistory.balanceAfter_usd)))
            {
                if (Update_depositWithdrawal_dbt(deposit_req, deposit_req.quantity_request))
                {
                    GlobalVariables::accounts[deposit_req.account_id].balance_usdt = walletHistory.balanceAfter_usd;
                    result = true;

                    if (!deposit_req.is_node)
                    {
                        walletHistory.account_id = market.accountID_TradeWith_TradeIsNotActive;
                        walletHistory.balanceBefore_usd = GlobalVariables::accounts[walletHistory.account_id].balance_usdt;
                        walletHistory.balanceAfter_usd = GlobalVariables::accounts[walletHistory.account_id].balance_usdt - deposit_req.quantity_request;

                        result = Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_account_balance_usdt(walletHistory.account_id, walletHistory.balanceAfter_usd));
                    }
                }
            }
        }
        else
        {
            walletHistory.account_id = deposit_req.account_id;
            double account_coin_walletQty = UtilityTools::Get_wallet_quantity(deposit_req.account_id, deposit_req.coin_id);
            walletHistory.coin_id = deposit_req.coin_id;
            walletHistory.coinBefore_qty = account_coin_walletQty;
            walletHistory.coinAfter_qty = account_coin_walletQty + deposit_req.quantity_request;
            if (Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_wallet_quantity(walletHistory.account_id, walletHistory.coin_id, walletHistory.coinAfter_qty)))
            {
                if (Update_depositWithdrawal_dbt(deposit_req, deposit_req.quantity_request))
                {
                    UtilityTools::Set_wallet_quantity(deposit_req.account_id, deposit_req.coin_id, walletHistory.coinAfter_qty);
                    result = true;

                    if (!deposit_req.is_node)
                    {
                        walletHistory.account_id = market.accountID_TradeWith_TradeIsNotActive;
                        double account_coin_walletQty = UtilityTools::Get_wallet_quantity(walletHistory.account_id, deposit_req.coin_id);
                        walletHistory.coin_id = deposit_req.coin_id;
                        walletHistory.coinBefore_qty = account_coin_walletQty;
                        walletHistory.coinAfter_qty = account_coin_walletQty - deposit_req.quantity_request;

                        result = Database_manager::walletHistory_dbt.Insert(walletHistory, UtilityTools::Set_wallet_quantity(walletHistory.account_id, walletHistory.coin_id, walletHistory.coinAfter_qty));
                    }
                }
            }
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

    return result;
}

bool DepositWithdrawalProcessor::Is_valid(DepositWithdrawal &depWit_req)
{
    bool result = false;
    try
    {
        if (depWit_req.coin_id == 0 || GlobalVariables::coins.find(depWit_req.coin_id) != GlobalVariables::coins.end())
        {
            result = true;
        }
        else
        {
            Update_depositWithdrawal_dbt(depWit_req, 0, rejectReason_DW_enum::CoinIsNotExist);
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

    return result;
}

unsigned DepositWithdrawalProcessor::ThreadFunction()
{
    while (isStopEventSignaled() == false)
    {
        {
            unique_lock lock(GlobalVariables::mut_depositWithdrawals);
            for (map<int, DepositWithdrawal>::iterator it_depoWithd = GlobalVariables::depositWithdrawals.begin();
                 it_depoWithd != GlobalVariables::depositWithdrawals.end(); it_depoWithd++)
            {
                if (!it_depoWithd->second.processed)
                {
                    if (Is_valid(it_depoWithd->second))
                    {
                        if (it_depoWithd->second.type == type_DW_enum::Deposit)
                        {
                            Deposit(it_depoWithd->second);
                        }
                        else
                        {
                            Withdrawal(it_depoWithd->second);
                        }
                    }
                    it_depoWithd->second.processed = true;
                }
            }
        }

        Loop_delay();
    }
    return 0;
}

void DepositWithdrawalProcessor::Loop_delay()
{
    static time_t time_lastGottentime = time(0);

    do
    {
        this_thread::sleep_for(chrono::milliseconds(50));
    } while (time(0) - time_lastGottentime < 1);

    time_lastGottentime = time(0);
}