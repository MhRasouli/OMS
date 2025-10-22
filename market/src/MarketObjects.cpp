#include "MarketObjects.h"

void MarketObjects::FreeMemory()
{
    delete pythonInterface;
    delete marketWalletsSupervisor;
    delete DW_Processor;
}