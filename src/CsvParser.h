#pragma once
#include "OrderBook.h"
#include "Order.h"
#include <string>

class CsvParser {
    public:
        void parseFile(const std::string& filename, OrderBook& orderBook);
};
