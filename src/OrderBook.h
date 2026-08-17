#pragma once
#include <unordered_map>
#include <list>
#include <map>
#include <cstdint>
#include <functional>
#include "Order.h"

struct ExecutionResult {
    uint64_t filledQuantity;
    uint64_t remainingQuantity;
    bool restedInBook;
    uint64_t restingOrderId; // ID of the resting order if any, 0 if none
};
class OrderBook {
    private:

        uint64_t nextOrderId = 001;

        std::map<int, std::list<Order>, std::greater<int>> bids; // Key: price, Value: list of orders at that price // greater<int> is used to sort the bids in descending order (highest price first)
        std::map<int, std::list<Order>> asks; // Key: price, Value: list of orders at that price

        std::unordered_map<uint64_t, std::list<Order>::iterator> bidOrderLookup; // Key: order ID, Value: iterator to the order in the bids list
        std::unordered_map<uint64_t, std::list<Order>::iterator> askOrderLookup; // Key: order ID, Value: iterator to the order in the asks list

    public:
        uint64_t addOrder(Order order);
        bool cancelOrder(uint64_t orderId);
        ExecutionResult executeOrder(Order order);
        void printOrderBook() const; // Optional: to visualize the current state of the order book
};

