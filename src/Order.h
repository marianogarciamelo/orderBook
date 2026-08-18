#pragma once
#include <cstdint>

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

enum class ActionType {
    ADD,
    CANCEL,
    EXECUTE
};

struct Order {
    uint64_t id;
    Side side;
    OrderType type;
    int price; // For LIMIT orders, this is the limit price. For MARKET orders, this can be ignored.
    uint32_t quantity; // The quantity of the order.
};