#include "CsvParser.h"
#include "Order.h"
#include "OrderBook.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

void CsvParser::parseFile(const std::string& filepath, OrderBook& orderBook) {
    std::ifstream file(filepath); //ifstream = input file stream, used to read from files
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return;
    }

    std::unordered_map<uint64_t, uint64_t> externalToInternalId; // Map to store external to internal order ID mapping

    std::string line;
    // Skip the header line
    bool isHeader = true;
    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }
        
        std::stringstream ss(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        std::string action = fields[1];
        uint64_t externalId = std::stoull(fields[2]);

        if (action == "ADD") {
            Order newOrder;
            newOrder.side = (fields[3] == "BUY") ? Side::BUY : Side::SELL;
            newOrder.price = std::stoi(fields[4]);
            newOrder.quantity = std::stoi(fields[5]);

            ExecutionResult result = orderBook.executeOrder(newOrder);

            if (result.restedInBook) {
                externalToInternalId[externalId] = result.restingOrderId;
            }

        } else if (action == "CANCEL") {
            uint64_t internalId = externalToInternalId[externalId];
            orderBook.cancelOrder(internalId);
        
        }  /* else if (action == "EXECUTE") {
            Order newOrder;
            newOrder.side = (fields[3] == "BUY") ? Side::BUY : Side::SELL;
            newOrder.price = std::stoi(fields[4]);
            newOrder.quantity = std::stoi(fields[5]);

            ExecutionResult result = orderBook.executeOrder(newOrder);

            if (result.restedInBook) {
                externalToInternalId[externalId] = result.restingOrderId;
            }
        } */
    }
}