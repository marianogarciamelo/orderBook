#include <iostream>
#include "Order.h"
#include "OrderBook.h"
#include "CsvParser.h"
#include "benchmark.h"
#include "PooledList.h"

int main() {
   
    OrderBook ob;
    CsvParser parser;
    //parser.parseFile("../data/orders.csv", ob);
    //ob.printOrderBook();
    std::vector<CsvEvent> events;
    parser.parseFiletoVector(std::string(PROJECT_SOURCE_DIR) + "/data/orders.csv", events);
    std::vector<long long> latenciesNs;
    std::vector<uint32_t> matchIterationCounts;              // <-- declared here, starts empty
    parser.processEvents(events, ob, latenciesNs, matchIterationCounts);  // <-- pass the vector to processEvents
    ob.printStatistics();  // <-- print statistics after processing events
    runBenchmarks(latenciesNs);  // <-- pass the vector to the benchmark function
    runMatchIterationStats(matchIterationCounts, latenciesNs);  // <-- pass the vector to the match iteration stats function

    return 0;
              
}