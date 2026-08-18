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

        std::cout << "Order " << order.id << " added: "
              << (order.side == Side::BUY ? "BUY" : "SELL")
              << " " << order.quantity << " @ " << order.price << "\n";

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
    totalCancelled++; // Increment the total canceled orders
    return true; // Order canceled successfully
}

ExecutionResult OrderBook::executeOrder(Order order) {
    uint64_t originalQuantity = order.quantity;
    bool restedInBook = false;
    uint64_t newID = 0; // Initialize newID to 0
    totalOrdersProcessed++; // Increment the total orders processed
    totalVolumeSubmitted += order.quantity; // Increment the total volume submitted

    std::cout << "Executing order: " << (order.side == Side::BUY ? "BUY" : "SELL")
              << " " << order.quantity << " @ " << order.price << "\n";

    if (order.side == Side::BUY) {
        while (order.quantity > 0 && !asks.empty()) {
            auto bestAskIt = asks.begin();
            int bestAskPrice = bestAskIt->first;
            auto& bestAskOrders = bestAskIt->second; //pointer to the real list of orders so we can modify it directly

            while (order.quantity > 0 && !bestAskOrders.empty()){
                Order& restingOrder = bestAskOrders.front(); // Get the first order in the list

                if (order.quantity < restingOrder.quantity) {
                    restingOrder.quantity -= order.quantity;
                    order.quantity = 0;
                } else {
                    order.quantity -= restingOrder.quantity;
                    askOrderLookup.erase(restingOrder.id); // Remove from lookup
                    bestAskOrders.pop_front(); // Remove the order from the list
                }
            }

            if (bestAskOrders.empty()) {
                asks.erase(bestAskPrice); // Remove the price level if there are no more orders at that price
            }
        }
        if (order.quantity > 0) {
            std::cout << "Not enough liquidity to execute the order. Setting up a bid for the remaining quantity of " << order.quantity << " shares @ " << order.price << ".\n";
            newID = addOrder(order); // Not enough liquidity to execute the order
            restedInBook = true;
        }  else {
            std::cout << "Order fully filled: " << (order.side == Side::BUY ? "BUY" : "SELL")
               << " " << originalQuantity << " @ " << order.price << ".\n";
        }

    } else if (order.side == Side::SELL) {
        while (order.quantity > 0 && !bids.empty()) {
            auto bestBidIt = bids.begin();
            int bestBidPrice = bestBidIt->first;
            auto& bestBidOrders = bestBidIt->second; //pointer to the real list of orders so we can modify it directly

            while (order.quantity > 0 && !bestBidOrders.empty()){
                Order& restingOrder = bestBidOrders.front(); // Get the first order in the list

                if (order.quantity < restingOrder.quantity) {
                    restingOrder.quantity -= order.quantity;
                    order.quantity = 0;
                } else {
                    order.quantity -= restingOrder.quantity;
                    bidOrderLookup.erase(restingOrder.id); // Remove from lookup
                    bestBidOrders.pop_front(); // Remove the order from the list
                }
            }

            if (bestBidOrders.empty()) {
                bids.erase(bestBidPrice); // Remove the price level if there are no more orders at that price
            }
            
        }

        if (order.quantity > 0) {
            std::cout << "Not enough liquidity to execute the order. Setting up an ask for the remaining quantity of " << order.quantity << " shares @ " << order.price << ".\n";
            newID = addOrder(order); // Not enough liquidity to execute the order
            restedInBook = true;
        } else {
            std::cout << "Order fully filled: " << (order.side == Side::BUY ? "BUY" : "SELL")
               << " " << originalQuantity << " @ " << order.price << ".\n";
        }
    }

    uint64_t filled = originalQuantity - order.quantity;
    totalFilled += filled; // Increment the total filled quantity
    if (restedInBook) {
        totalRested++;
    }

    return ExecutionResult{
        originalQuantity - order.quantity, //filled quantity
        order.quantity, // remaining quantity
        restedInBook,
        restedInBook ? newID : 0 // resting order ID if any, 0 if none


    };
}

void OrderBook::printOrderBook() const {
    std::cout << "Order Book:\n";
    std::cout << "Bids:\n";
    for (const auto& [price, orders] : bids) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << ", Quantity: " << order.quantity << ", Price: " << price << "\n";
        }
    }

    std::cout << "Asks:\n";
    for (const auto& [price, orders] : asks) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << ", Quantity: " << order.quantity << ", Price: " << price << "\n";
        }
    }
}

void OrderBook::printStatistics() const {
    std::cout << "\n=== Order Book Statistics ===\n";
    std::cout << "Total Orders Processed: " << totalOrdersProcessed << "\n";
    std::cout << "Total Volume / Quantity Submitted: " << totalVolumeSubmitted << "\n";
    std::cout << "Total Filled Quantity: " << totalFilled << "\n";
    std::cout << "Total Cancelled Orders: " << totalCancelled << "\n";
    std::cout << "Total Rested Orders: " << totalRested << "\n";

    if (totalOrdersProcessed > 0) {
        double restRate = static_cast<double>(totalRested) / totalOrdersProcessed * 100.0;
        std::cout << "Rest Rate: " << restRate << "%\n";
    }

    if (totalVolumeSubmitted > 0) {
        double fillRate = static_cast<double>(totalFilled) / totalVolumeSubmitted * 100.0;
        std::cout << "Fill Rate: " << fillRate << "%\n";
    }
}