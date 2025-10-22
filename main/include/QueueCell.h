#pragma once
#include "Enums.h"
#include "Trade.h"


class QueueCell
{
public:
	Enum_QueueCellType queueCellType;
	QueueCell(Enum_QueueCellType QueueCellType);
};

class QueueCell_Trade : public QueueCell
{
public:
	Trade trade;
	QueueCell_Trade(Trade _trade, Enum_QueueCellType QueueCellType);
};