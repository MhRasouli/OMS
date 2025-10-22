#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "DepositWithdrawal.h"
#include "Market.h"

class DepositWithdrawalProcessor : public AThread
{
public:
	DepositWithdrawalProcessor(Market _market);

private:
	Market market;
	unsigned ThreadFunction();
	void Loop_delay();
	bool Deposit(DepositWithdrawal &deposit_req);
	bool Is_valid(DepositWithdrawal &depWit_req);
	bool Withdrawal(DepositWithdrawal &withdrawal_req);
	bool Update_depositWithdrawal_dbt(DepositWithdrawal &depoWithd_req, double final_qty, rejectReason_DW_enum rejectReason_id);
};