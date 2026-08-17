#include <fstream>
#include <random>
#include <iostream>

int main() {
    const int numOrders = 1'000'000;

    std::ofstream file("../data/orders.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return 1;
}
    file << "timestamp,action,order_id,side,price,quantity\n";
    
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> sideDist(0, 1); // 0 for BUY, 1 for SELL
    std::uniform_int_distribution<int> priceDist(10000, 15000); // Price range
    std::uniform_int_distribution<int> quantityDist(1, 1000); // Quantity

    for (int i = 0; i < numOrders; ++i) {
        std::string side = sideDist(rng) == 0 ? "BUY" : "SELL";
        int price = priceDist(rng);
        int qty = quantityDist(rng);
        // timestamp = i, order_id = i+1 (external ID, distinct from book's internal ID)
        file << i << ",ADD," << (i + 1) << "," << side << "," << price << "," << qty << "\n";
    }
    file.close();
    std::cout << "Wrote " << numOrders << " orders to ../data/orders.csv\n";
    return 0;

}