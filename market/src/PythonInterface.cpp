// PythonInterface

#include "PythonInterface.h"
#include "GlobalVariables.h"

PythonInterface::PythonInterface(Market _market) : market(_market)
{
}

unsigned PythonInterface::ThreadFunction()
{
    // Interpreter must be the first thing created
    py::scoped_interpreter guard{};
    py::gil_scoped_acquire gil;
    try
    {
        // All Python objects live inside this block so they are destroyed
        // before the guard finalizes the interpreter
        py::module sys = py::module::import("sys");
        sys.attr("path").cast<py::list>().append(".");

        py::module kucoin_module = py::module::import("KucoinLibrary");
        py::object KucoinMarket_class = kucoin_module.attr("KucoinMarket");

        {
            py::gil_scoped_acquire gil;
            GlobalVariables::kucoinMarketInterface = KucoinMarket_class(
                market.username_privateKey.c_str(),
                market.password_securityKey.c_str(),
                market.passphrase.c_str());
        }

        while (!isStopEventSignaled())
        {
            std::unique_lock<std::mutex> lock(GlobalVariables::queueMutex);
            GlobalVariables::cv.wait(lock, [this]
                                     { return this->stopped || !GlobalVariables::taskQueue.empty(); });

            if (stopped && GlobalVariables::taskQueue.empty())
                break;

            auto task = std::move(GlobalVariables::taskQueue.front());
            GlobalVariables::taskQueue.pop();
            lock.unlock();

            {
                // Ensure Python calls happen with GIL
                py::gil_scoped_acquire gil;
                task();
            }
        }

        {
            py::gil_scoped_acquire gil;
            GlobalVariables::kucoinMarketInterface = py::none();
        }
    } // all py::object destroyed here, while interpreter still alive
    catch (const py::error_already_set &e)
    {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
    GlobalVariables::cv.notify_all();

    return 0;
}