# Kucoin WebSocket Integration

This document describes the new Kucoin WebSocket classes that have been integrated into the BinanceTradingService project to replace the Python-based KucoinFeeder.

## Overview

The new classes provide direct WebSocket connectivity to Kucoin's market data feed, offering real-time price updates without relying on the Python interface. The existing `KucoinFeeder` class has been updated to use these new WebSocket components.

## Classes

### 1. KucoinTokenManager

Handles authentication and token management for Kucoin WebSocket connections.

**Features:**
- Automatic token renewal every 23 hours
- Thread-safe token access
- Proper cleanup and resource management

### 2. KucoinWebSocketFeed

Handles WebSocket connections and market data processing.

**Features:**
- Real-time market data streaming
- Automatic ping/pong handling
- Configurable price update callbacks with bid/ask data
- Thread-safe operation
- Proper connection cleanup

### 3. Updated KucoinFeeder

The existing `KucoinFeeder` class has been enhanced to use WebSocket instead of Python interface.

**Changes:**
- Replaced Python interface with direct WebSocket connection
- Real-time price updates instead of polling
- Better error handling and reconnection logic
- Maintains compatibility with existing `PriceFeeder` interface

## Integration with Existing System

The new WebSocket classes are now fully integrated into the existing trading system:

1. **Automatic Integration:**
   - The `PriceFeederManager` automatically creates `KucoinFeeder` instances for Kucoin coins
   - No changes needed to existing code that uses `PriceFeeder`

2. **Database Updates:**
   - Price updates automatically flow to the database through existing mechanisms
   - Coin objects are updated with real-time bid/ask prices and quantities

3. **Trading Logic:**
   - Existing trading algorithms continue to work unchanged
   - Real-time data provides faster and more accurate price information

## Usage

The integration is automatic. When the system starts:

1. `PriceFeederManager` detects coins with `market_id` pointing to "Kucoin"
2. Creates `KucoinFeeder` instances for each coin
3. `KucoinFeeder` automatically:
   - Initializes `KucoinTokenManager` for authentication
   - Establishes WebSocket connection via `KucoinWebSocketFeed`
   - Subscribes to real-time ticker data for the coin
   - Updates coin prices in the database via callbacks

## Thread Safety

All classes are designed to be thread-safe:
- `KucoinTokenManager` uses mutex protection for shared data
- `KucoinWebSocketFeed` uses atomic flags for state management
- `KucoinFeeder` maintains compatibility with existing threading model
- Proper cleanup prevents resource leaks

## Dependencies

The classes require:
- `websocketpp` library
- `nlohmann/json` for JSON parsing
- `libcurl` for HTTP requests
- `boost::system` for ASIO support

## Building

The classes are automatically included in the build when you compile the BinanceTradingService project. Make sure all dependencies are installed:

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libwebsocketpp-dev nlohmann-json3-dev libcurl4-openssl-dev libboost-system-dev

# Build the project
cd build
cmake ..
make
```

## Performance Improvements

- **Real-time Updates:** No more polling delays
- **Lower Latency:** Direct WebSocket connection
- **Better Reliability:** Automatic reconnection and error handling
- **Reduced Resource Usage:** No Python interpreter overhead

## Error Handling

The classes include comprehensive error handling:
- Network connection failures
- JSON parsing errors
- WebSocket protocol errors
- Graceful shutdown on errors
- Automatic reconnection attempts

## Migration Notes

The migration from Python-based to WebSocket-based is transparent:
- No changes needed to existing trading logic
- Database schema remains the same
- All existing APIs continue to work
- Improved performance and reliability 