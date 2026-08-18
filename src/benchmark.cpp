#include "benchmark.h"
#include <algorithm>
#include <iostream>
#include <numeric>

void runBenchmarks(std::vector<long long>& latenciesNs) {
    if (latenciesNs.empty()) {
        std::cerr << "No latency data to analyze." << std::endl;
        return;
    }

    std::sort(latenciesNs.begin(), latenciesNs.end());

    size_t n = latenciesNs.size();

    long long sum = std::accumulate(latenciesNs.begin(), latenciesNs.end(), 0LL);
    double mean = static_cast<double>(sum) / n;

    long long median = latenciesNs[n / 2];
    long long p95 = latenciesNs[static_cast<size_t>(n * 0.95)];
    long long p99 = latenciesNs[static_cast<size_t>(n * 0.99)];
    long long max = latenciesNs[n - 1];

    std::cout << "Operations: " << n << "\n";
    std::cout << "Mean:   " << mean   << " ns\n";
    std::cout << "Median: " << median << " ns\n";
    std::cout << "p95:    " << p95    << " ns\n";
    std::cout << "p99:    " << p99    << " ns\n";
    std::cout << "Max:    " << max    << " ns\n";
}