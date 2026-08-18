#include "CsvParser.h"
#include "Order.h"
#include "OrderBook.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>

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

void CsvParser::parseFiletoVector(const std::string& filepath, std::vector<CsvEvent>& events) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return;
    }

    std::string line;
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
        // Convert action (fields[1])
        ActionType action = (fields[1] == "ADD") ? ActionType::ADD : ActionType::CANCEL;
        // Convert order_id (fields[2])
        uint64_t externalId = std::stoull(fields[2]);
        // Convert side (fields[3])
        Side side = (fields[3] == "BUY") ? Side::BUY : Side::SELL;
        // Convert price (fields[4])
        int price = std::stoi(fields[4]);
        // Convert quantity (fields[5])
        uint32_t quantity = static_cast<uint32_t>(std::stoul(fields[5]));

        // Build the Order
        Order order;
        order.id = externalId;
        order.side = side;
        order.type = OrderType::LIMIT;   // your CSV only has LIMIT orders currently
        order.price = price;
        order.quantity = quantity;

        // Build the CsvEvent and store it
        CsvEvent event;
        event.action = action;
        event.order = order;
        events.push_back(event);

        }
    }

    void CsvParser::processEvents(const std::vector<CsvEvent>& events, OrderBook& orderBook, std::vector<long long>& latenciesNs) {
        std::unordered_map<uint64_t, uint64_t> externalToInternalId;
        for (const auto& event : events) {
            if (event.action == ActionType::ADD) {
                auto start = std::chrono::high_resolution_clock::now();
                ExecutionResult result = orderBook.executeOrder(event.order);
                auto end = std::chrono::high_resolution_clock::now();

                latenciesNs.push_back(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                );

                if (result.restedInBook) {
                    externalToInternalId[event.order.id] = result.restingOrderId;
                }
            } else if (event.action == ActionType::CANCEL) {
                auto it = externalToInternalId.find(event.order.id);
                if (it != externalToInternalId.end()) {
                    auto start = std::chrono::high_resolution_clock::now();
                    orderBook.cancelOrder(it->second);
                    auto end = std::chrono::high_resolution_clock::now();

                    latenciesNs.push_back(
                        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                    );
                }
            }
        }
    }