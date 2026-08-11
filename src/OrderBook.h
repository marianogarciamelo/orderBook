#pragma once
#include <unordered_map>
#include <list>
#include <map>
#include <cstdint>
#include "Order.h"

class OrderBook {
    private:

        uint64_t nextOrderId = 001;

        std::map<int, std::list<Order>> bids; // Key: price, Value: list of orders at that price
        std::map<int, std::list<Order>> asks; // Key: price, Value: list of orders at that price

        std::unordered_map<uint64_t, std::list<Order>::iterator> bidOrderLookup; // Key: order ID, Value: iterator to the order in the bids list
        std::unordered_map<uint64_t, std::list<Order>::iterator> askOrderLookup; // Key: order ID, Value: iterator to the order in the asks list

    public:
        uint64_t addOrder(Order order);
};

