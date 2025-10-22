#include "QueueCell.h"
#include "Trade.h"



QueueCell::QueueCell( Enum_QueueCellType QueueCellType)
{
	queueCellType = QueueCellType;
}

QueueCell_Trade::QueueCell_Trade(Trade _trade, Enum_QueueCellType QueueCellType) :QueueCell(QueueCellType), trade(_trade)
{
}