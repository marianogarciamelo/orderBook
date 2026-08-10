#include <iostream>
#include "Order.h"

int main() {
    Order o {1, Side::BUY, OrderType::LIMIT, 101.50, 100};
    std::cout <<"Order " << o.id << ": "
              << (o.side == Side::BUY ? "BUY" : "SELL")
              << " " << (o.type == OrderType::LIMIT ? "LIMIT" : "MARKET") 
              << " " << o.quantity << " @ " << o.price << "\n";
    return 0;
              
}