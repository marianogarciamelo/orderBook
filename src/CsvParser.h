#pragma once
#include "OrderBook.h"
#include "Order.h"
#include <string>
#include <vector>

struct CsvEvent {
    ActionType action;
    Order order;       // fully populated for ADD; for CANCEL you might only need order.id
};
class CsvParser {
    public:
        void parseFile(const std::string& filename, OrderBook& orderBook);
        void parseFiletoVector(const std::string& filename, std::vector<CsvEvent>& events);
        void processEvents(const std::vector<CsvEvent>& events, OrderBook& orderBook, std::vector<long long>& latenciesNs, std::vector<uint32_t>& matchIterationsCounts);
};
