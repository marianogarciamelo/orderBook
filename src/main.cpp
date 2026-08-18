#include <iostream>
#include "Order.h"
#include "OrderBook.h"
#include "CsvParser.h"
#include "benchmark.h"

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

    uint64_t id1 = ob.addOrder(o);
    std::cout << "Added order, assigned id: " << id1 << "\n";

    uint64_t id2 = ob.addOrder(o2);
    std::cout << "Added order, assigned id: " << id2 << "\n";

    bool result1 = ob.cancelOrder(id1);
    std::cout << "Cancel result: " << (result1 ? "true" : "false") << "\n";

    bool result3 = ob.cancelOrder(id2);
    std::cout << "Cancel result: " << (result3 ? "true" : "false") << "\n";

    bool result2 = ob.cancelOrder(9999);  // never assigned, should fail
    std::cout << "Cancel result: " << (result2 ? "true" : "false") << "\n";
    

    CsvParser parser;
    //parser.parseFile("../data/orders.csv", ob);
    //ob.printOrderBook();
    std::vector<CsvEvent> events;
    parser.parseFiletoVector(std::string(PROJECT_SOURCE_DIR) + "/data/orders.csv", events);
    std::vector<long long> latenciesNs;              // <-- declared here, starts empty
    parser.processEvents(events, ob, latenciesNs);
    ob.printStatistics();  // <-- print statistics after processing events
    runBenchmarks(latenciesNs);  // <-- pass the vector to the benchmark function

    return 0;
              
}