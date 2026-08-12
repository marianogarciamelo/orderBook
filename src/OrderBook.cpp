#include "OrderBook.h"
#include <iterator>
#include <iostream>

uint64_t OrderBook::addOrder(Order order) { // :: reads as "OrderBook::addOrder" and is the implementation of the addOrder method of the OrderBook class
        order.id = nextOrderId++;

        if (order.side == Side::BUY) {
            bids[order.price].push_back(order);
            bidOrderLookup[order.id] = std::prev(bids[order.price].end()); // Store the iterator to the newly added order in the bidOrderLookup map

        } else if (order.side == Side::SELL) {
            asks[order.price].push_back(order);
            askOrderLookup[order.id] = std::prev(asks[order.price].end()); // Store the iterator to the newly added order in the askOrderLookup map

        }

        return order.id;

}

bool OrderBook::cancelOrder(uint64_t orderId) {
    // it is an iterator (bookmark) pointing at the actual Order struct in the list.
    // -> dereferences it, then reads .price off that struct - same struct that
    // was inserted back in addOrder. We must read this BEFORE erasing, since
    // erase(it) invalidates the iterator afterward.
    if (bidOrderLookup.count(orderId) > 0) {
        auto it = bidOrderLookup[orderId];
        int price = it->price; // dereference the iterator to get the order and then access its price
        bids[price].erase(it);
        bidOrderLookup.erase(orderId);

        if (bids[price].empty()) {
            bids.erase(price); // Remove the price level if there are no more orders at that price
        }

    } else if (askOrderLookup.count(orderId) > 0) {
        auto it = askOrderLookup[orderId];
        int price = it->price; // dereference the iterator to get the order and then access its price
        asks[price].erase(it);
        askOrderLookup.erase(orderId);

        if (asks[price].empty()) {
            asks.erase(price); // Remove the price level if there are no more orders at that price
        }

    } else {
        std::cout << "Order ID " << orderId << " not found.\n";
        return false; // Order ID not found
    }
    std::cout << "Order " << orderId << " canceled successfully.\n";
    return true; // Order canceled successfully
}