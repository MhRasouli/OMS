#include "KucoinFeederPython.h"
#include "UtilityTools.h"
#include "Enums.h"
#include "Logger.h"

KucoinFeederPython::~KucoinFeederPython()
{
    fedchkThread.Stop();
    StopGetData();
}

KucoinFeederPython::KucoinFeederPython(Coin *Coin_ptr) : coin_ptr(Coin_ptr), fedchkThread(&dataIsNew, Coin_ptr, &mustDelete)
{
}

unsigned KucoinFeederPython::ThreadFunction()
{
    std::map<std::string, std::string> result;
    //**************************************************************************** */
    fedchkThread.Start("Feed checker symbol: " + coin_ptr->name);
    while (isStopEventSignaled() == false)
    {
        try
        {
            // Package the task for methodB with input
            string coin_name = coin_ptr->name;
            std::packaged_task<std::map<std::string, std::string>()> task(
                [coin_name]()
                {
                    std::map<std::string, std::string> cpp_map;
                    try
                    {
                        const py::object py_obj = GlobalVariables::kucoinMarketInterface.attr("SymbolPrice")(UtilityTools::string_attachEnd(coin_name, "-USDT").c_str());

                        if (py::isinstance<py::dict>(py_obj))
                        {
                            py::dict py_dict = py_obj.cast<py::dict>();

                            for (auto item : py_dict)
                            {
                                std::string key = py::str(item.first);    // Convert Python key to std::string
                                std::string value = py::str(item.second); // Convert Python value to std::string
                                cpp_map[key] = value;                     // Insert key-value pair into std::map
                            }
                        }
                        else
                        {
                            Logger::logger.log_special(LogLocation, true, "The function did not return a dictionary!");
                            // std::cerr << "The function did not return a dictionary!" << std::endl;
                        }
                    }
                    catch (const py::error_already_set &e)
                    {
                        Logger::logger.log_special(LogLocation, true, "Python error: %s",e.what());
                        //std::cerr << "Python error: " << e.what() << std::endl;
                    }

                    return cpp_map;
                });

            // Get future to retrieve the result
            std::future<std::map<std::string, std::string>> resultFuture = task.get_future();

            // Send the task to thread A via the task queue
            {
                std::lock_guard<std::mutex> lock(GlobalVariables::queueMutex);
                GlobalVariables::taskQueue.push(std::move(task));
            }

            // Notify thread A that a new task is available
            GlobalVariables::cv.notify_one();

            // Wait for the result from thread A
            result = resultFuture.get();

            //**************************************************************************************************
            // coin_ptr->market_buy_price = atof(result["bestAsk"].c_str());
            // coin_ptr->market_buy_quantity = atof(result["bestAskSize"].c_str());

            // coin_ptr->market_sell_price = atof(result["bestBid"].c_str());
            // coin_ptr->market_sell_quantity = atof(result["bestBidSize"].c_str());

            coin_ptr->market_buy_price = atof(result["bestBid"].c_str());
            coin_ptr->market_buy_quantity = atof(result["bestBidSize"].c_str());

            coin_ptr->market_sell_price = atof(result["bestAsk"].c_str());
            coin_ptr->market_sell_quantity = atof(result["bestAskSize"].c_str());

            // std::cout << "Price update - Symbol: " << coin_name
            //           << ", Best Bid: " << coin_ptr->market_buy_price << " (Size: " << coin_ptr->market_buy_quantity << ")"
            //           << ", Best Ask: " << coin_ptr->market_sell_price << " (Size: " << coin_ptr->market_sell_quantity << ")" << std::endl;

            //*************************************************************************************************
            coin_ptr->time_price = time(0);
            dataIsNew = true;
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
        Loop_delay();
    }

    return 0;
}

void KucoinFeederPython::StopGetData()
{
    Stop();
}

void KucoinFeederPython::Loop_delay()
{
    static time_t time_lastGottentime = time(0);
    do
    {
        this_thread::sleep_for(chrono::milliseconds(50));
    } while (time(0) - time_lastGottentime < 1);

    time_lastGottentime = time(0);
}
