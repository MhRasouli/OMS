#include "GlobalVariables.h"

int GlobalVariables::MainLoopCounter = 0;

std::queue<std::packaged_task<std::map<std::string, std::string>()>> GlobalVariables::taskQueue;
std::mutex GlobalVariables::queueMutex;
std::condition_variable GlobalVariables::cv;
py::object GlobalVariables::kucoinMarketInterface;
map<int, Coin> GlobalVariables::coins;
map<int, Account> GlobalVariables::accounts;
map<int, TradeRequestGenerator> GlobalVariables::tradeRequestGenerators;
map<int, Change> GlobalVariables::changes;
map<int, Market> GlobalVariables::markets;
PriceFeederManager GlobalVariables::priceFeederManager;
map<int, AccountTpSl> GlobalVariables::accountTpSls;
map<int, CommissionRate> GlobalVariables::commissionRates;
list<StairCommission> GlobalVariables::stairCommissions;
map<int, DepositWithdrawal> GlobalVariables::depositWithdrawals;
shared_mutex GlobalVariables::mut_depositWithdrawals;
map<int, CoinMinBalance_alertMessageThreshold> GlobalVariables::coinMinBalanceThresholds;
map<int, Group> GlobalVariables::groups;
map<int, BuyLimitation> GlobalVariables::buyLimitations;
map<pair<int, int>, GroupCoinMarketCommission> GlobalVariables::GroupCoinMarketCommissions;
map<int, TradeRequestRobot> GlobalVariables::tradeRequestRobots;
map<int, LastTradePriceGenerator> GlobalVariables::lastTradePriceGenerators;
map<int, CommissionRateRule> GlobalVariables::commissionRateRules;
map<int,MarketObjects> GlobalVariables::marketObjects;