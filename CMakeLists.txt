# CMakeLists.txt : CMake project for BinanceTradingService, includes source and defines
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

project(BinanceTradingService)

set(pybind11_DIR /usr/lib/cmake/pybind11)

find_package(pybind11 REQUIRED)
find_package(Python3  REQUIRED)
find_package(nlohmann_json REQUIRED)

# Set C++ Standard and Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb3 -std=c++17 -O0")  # -O0 for faster debug builds

# Enable Precompiled Headers
#target_precompile_headers(${PROJECT_NAME} PRIVATE "./main/include/stdafx.h")

# Enable Ninja or Multi-threaded Build with Make
if(CMAKE_GENERATOR STREQUAL "Ninja")
    message(STATUS "Using Ninja build system for faster builds")
elseif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(CMAKE_MAKE_PROGRAM "make -j$(nproc)")
endif()

# Enable Incremental Linking for faster link times
# set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL")

# Enable ccache for faster rebuilds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache for faster rebuilds")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

#-------------------------------------------------------------------------------------------------------------
# Multi-Configuration Setup
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
    if(NOT "Asan" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Asan)
    endif()
else()
    set(allowedBuildTypes Asan Debug Release RelWithDebInfo MinSizeRel)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")
    if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
        message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# AddressSanitizer Flags
set(CMAKE_C_FLAGS_ASAN "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING "Flags for ASan." FORCE)
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING "Flags for ASan." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING "Linker flags for ASan." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_ASAN "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING "Shared library linker flags for ASan." FORCE)
#-------------------------------------------------------------------------------------------------------------

# Add Preprocessor Definitions
add_definitions(
    -UNDEBUG
    -DDTF_HEADER_ONLY
    -D__FLATJSON__CHILDS_TYPE=std::uint32_t
    -D__FLATJSON__VLEN_TYPE=std::uint32_t
)

# Include Directories
include_directories(
    ${Python3_INCLUDE_DIRS}
    /usr/local/include
    /usr/include
    /usr/include/mysql-cppconn
    /usr/include/mysql-cppconn/mysql
    /usr/include/mysql-cppconn/jdbc/cppconn
    /usr/include/cppconn
    /usr/include/jsoncpp
    /usr/include/websocketpp
    ./binapi
    ./main/include
    ./Database/include
    ./logger/include
    ./container/include
    ./market/include
    ./MemoryMonitor/include
    ./communicationInterface/include
)

# Optional Boost Include Directory
if (DEFINED ${BOOST_INCLUDE_DIR})
    include_directories(${BOOST_INCLUDE_DIR})
endif()


# Source Files
set(BINAPI_HEADERS
    include/api.hpp
    include/flatjson.hpp
    include/dtf.hpp
    include/double_type.hpp
    include/enums.hpp
    include/errors.hpp
    include/invoker.hpp
    include/message.hpp
    include/pairslist.hpp
    include/reports.hpp
    include/tools.hpp
    include/types.hpp
    include/websocket.hpp
)

set(MEMORYMONITOR_SOURCES)

set(BINAPI_SOURCES
    binapi/src/api.cpp
    binapi/src/enums.cpp
    binapi/src/errors.cpp
    binapi/src/pairslist.cpp
    binapi/src/reports.cpp
    binapi/src/tools.cpp
    binapi/src/types.cpp
    binapi/src/websocket.cpp
)

set(SOCKET_SOURCES
   socket/src/tcp_client.cpp
   socket/src/tcp_server.cpp
   socket/src/client.cpp
   socket/src/pipe_ret_t.cpp
   socket/src/common.cpp
)

set(MAIN_SOURCE
    main/src/main.cpp
    main/src/AThread.cpp
    main/src/CircularArray.cpp
    main/src/Configuration.cpp
    main/src/StdAfx.cpp
    main/src/GlobalVariables.cpp
    main/src/UtilityTools.cpp
    main/src/QueueCell.cpp
    main/src/ChangeProcessor.cpp
    main/src/TradeRequestProcessor.cpp
    main/src/DepositWithdrawalProcessor.cpp
    main/src/ApiInterface.cpp
)

set(DATABASE_SOURCE
    Database/src/Database_manager.cpp
    Database/src/DatabaseTable.cpp
    Database/src/Trade_dbt.cpp
    Database/src/TradeRequest_dbt.cpp
    Database/src/Account_dbt.cpp
    Database/src/Coin_dbt.cpp
    Database/src/Change_dbt.cpp
    Database/src/Wallet_dbt.cpp
    Database/src/Commission_dbt.cpp
    Database/src/MarketResponse_dbt.cpp
    Database/src/MarketResponseDetail_dbt.cpp
    Database/src/WalletHistory_dbt.cpp
    Database/src/TradeRequestGenerator_dbt.cpp
    Database/src/Market_dbt.cpp
    Database/src/Message_dbt.cpp
    Database/src/CommissionRate_dbt.cpp
    Database/src/StairCommission_dbt.cpp
    Database/src/DepositWithdrawal_dbt.cpp
    Database/src/CoinMinBalance_alertMessageThreshold_dbt.cpp
    Database/src/Group_dbt.cpp
    Database/src/BuyLimitation_dbt.cpp
    Database/src/GroupCoinMarketCommission_dbt.cpp
    Database/src/TradeRequestRobot_dbt.cpp
    Database/src/Prices_dbt.cpp
    Database/src/LastTradePriceGenerator_dbt.cpp
    Database/src/CommissionRateRule_dbt.cpp
)

set(LOGGER_SOURCE
    logger/src/Logger.cpp
)

set(CONTAINER_SOURCE
    container/src/TradeRequest.cpp
)

set(MARKET_SOURCE
   market/src/BinanceFeeder.cpp
   market/src/FeedCheckerThread.cpp
   market/src/BinanceTrader.cpp
   market/src/PriceFeederManager.cpp
   market/src/MarketWalletsSupervisor.cpp
   market/src/NobitexFeeder.cpp
   market/src/PriceFeeder.cpp
   market/src/MarketTrader.cpp
   market/src/NobitexTrader.cpp
   market/src/KucoinTrader.cpp
   market/src/KucoinFeeder.cpp
   market/src/KucoinTokenManager.cpp
   market/src/KucoinWebSocketFeed.cpp
   market/src/PythonInterface.cpp
   market/src/KucoinFeederPython.cpp
   market/src/MarketObjects.cpp
)

# Create Executable
add_executable(
    ${PROJECT_NAME}
    ${CONTAINER_SOURCE}
    ${MAIN_SOURCE}
    ${DATABASE_SOURCE}
    ${BINAPI_SOURCES}
    ${SOCKET_SOURCES}
    ${LOGGER_SOURCE}
    ${MARKET_SOURCE}
    ${MEMORYMONITOR_SOURCES}
)

# Optional Boost Library Linkage
if (DEFINED ${BOOST_LIB_DIR})
    target_link_libraries(${PROJECT_NAME} -L${BOOST_LIB_DIR})
endif()

# Link Libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Python3_LIBRARIES}
    pybind11::embed
    nlohmann_json::nlohmann_json
    stdc++
    mysqlcppconn
    z
    crypto
    ssl
    pthread
    /usr/lib/x86_64-linux-gnu/libjsoncpp.so
    /usr/lib/x86_64-linux-gnu/libjsonrpccpp-client.so
    /usr/lib/x86_64-linux-gnu/libjsonrpccpp-common.so
    /usr/lib/x86_64-linux-gnu/libjsonrpccpp-server.so
    /usr/lib/x86_64-linux-gnu/libjsonrpccpp-stub.so
    /usr/lib/x86_64-linux-gnu/libcpr.so.1
    /usr/lib/x86_64-linux-gnu/libcurl.so
    /usr/lib/x86_64-linux-gnu/libboost_system.so
)
