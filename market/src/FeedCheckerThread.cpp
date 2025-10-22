#include "FeedCheckerThread.h"

FeedCheckerThread::FeedCheckerThread(bool *DataIsNew, Coin *Coin_ptr, bool *Wsb_mustDelete) : dataIsNew(DataIsNew),
																							  coin_ptr(Coin_ptr),
																							  wsb_mustDelete(Wsb_mustDelete)
{
}

unsigned FeedCheckerThread::ThreadFunction()
{
	this_thread::sleep_for(chrono::milliseconds(10000));

	bool feedStopEdge = false;

	time_lastGottenData = time(nullptr);

	bool StopEventSignaled = false;
	while (StopEventSignaled == false)
	{
		while (!(*dataIsNew) && (StopEventSignaled = isStopEventSignaled()) == false)
		{
			if (coin_ptr->maxLimitFeedStop_sec > 0 && (time(nullptr) - time_lastGottenData > coin_ptr->maxLimitFeedStop_sec))
			{
				if (feedStopEdge == false)
				{
					feedStopEdge = true;
					Logger::logger.log_special(LogLocation, false, "The feed of symbol '%s' stopped.", coin_ptr->name.c_str());
				}
				*wsb_mustDelete = true;
				time_lastGottenData = time(nullptr);
			}

			this_thread::sleep_for(chrono::milliseconds(1000));
		}
		if (!isStopEventSignaled() && feedStopEdge == true)
		{
			feedStopEdge = false;
			Logger::logger.log_special(LogLocation, false, "The feed of symbol '%s' running.", coin_ptr->name.c_str());
		}
		*dataIsNew = false;
		time_lastGottenData = time(nullptr);
		//std::cout << "FeedCheckerThread detected new data for " << coin_ptr->name << std::endl;
	}

	return 0;
}
