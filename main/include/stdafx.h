#pragma once

#include <stdlib.h>
#include <iostream>
#include <driver.h>
#include <exception.h>
#include <resultset.h>
#include <statement.h>
#include <shared_mutex>
#include <cstdarg>
#include <thread>
#include <chrono>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <thread>
#include <functional>

#include <filesystem>
#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <queue>
#include <future>
#include <condition_variable>

using std::filesystem::current_path;
using namespace std;
namespace py = pybind11;

#define TwoKey(accountGroup_id,coin_id) std::make_pair(accountGroup_id,coin_id)

#pragma region
#define CATCH_STATEMENT                                                                                        \
	catch (const std::runtime_error &re)                                                                       \
	{                                                                                                          \
		Logger::logger.log_special(LogLocation, true, "Runtime error: %s", re.what());                         \
		throw std::runtime_error(re.what());                                                                   \
	}                                                                                                          \
	catch (const std::exception &ex)                                                                           \
	{                                                                                                          \
		Logger::logger.log_special(LogLocation, true, "Error occurred: %s", ex.what());                        \
		throw std::runtime_error(ex.what());                                                                   \
	}                                                                                                          \
	catch (...)                                                                                                \
	{                                                                                                          \
		Logger::logger.log_special(LogLocation, true, "Unknown failure occurred. Possible memory corruption"); \
		throw std::runtime_error("Unknown failure occurred. Possible memory corruption");                      \
	}
#pragma endregion catch statement region.