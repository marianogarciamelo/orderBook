#include <iostream>
#include "Order.h"
#include "OrderBook.h"

int main() {
    Order o {1, Side::BUY, OrderType::LIMIT, 10150, 100};
    std::cout <<"Order " << o.id << ": "
              << (o.side == Side::BUY ? "BUY" : "SELL")
              << " " << (o.type == OrderType::LIMIT ? "LIMIT" : "MARKET") 
              << " " << o.quantity << " @ " << o.price << "\n";

    Order o2 {4, Side::SELL, OrderType::MARKET, 20164, 100};
    std::cout <<"Order " << o2.id << ": "
              << (o2.side == Side::BUY ? "BUY" : "SELL")
              << " " << (o2.type == OrderType::LIMIT ? "LIMIT" : "MARKET") 
              << " " << o2.quantity << " @ " << o2.price << "\n";          
    
    OrderBook ob;
    
    return 0;
              
}