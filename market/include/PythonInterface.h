//PythonInterface
#pragma once

#include "stdafx.h"
#include "AThread.h"
#include <Market.h>

class PythonInterface : public AThread
{
public:
	PythonInterface(Market _market);
private:
Market market;
	unsigned ThreadFunction();
};
