#include "benchmark.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <iomanip>
#include <string>
#include <chrono>
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

void runMatchIterationStats(const std::vector<uint32_t>& matchIterations, const std::vector<long long>& latenciesNs) {
    if (matchIterations.empty()) return;

    uint64_t sum = 0;
    uint32_t maxIter = 0;
    for (auto m : matchIterations) {
        sum += m;
        maxIter = std::max(maxIter, m);
    }
    double mean = static_cast<double>(sum) / matchIterations.size();

    std::cout << "\n=== Match Iteration Statistics ===\n";
    std::cout << "Mean iterations per order: " << mean << "\n";
    std::cout << "Max iterations for a single order: " << maxIter << "\n";

    // Correlate: top 10 slowest latencies, show their match iteration count
    std::vector<size_t> idx(latenciesNs.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::partial_sort(idx.begin(), idx.begin() + 10, idx.end(),
        [&](size_t a, size_t b) { return latenciesNs[a] > latenciesNs[b]; });

    std::cout << "\nTop 10 slowest ops:\n";
    for (size_t i = 0; i < 10; ++i) {
        std::cout << "  latency=" << latenciesNs[idx[i]] << "ns, matchIterations=" 
                   << matchIterations[idx[i]] << "\n";
    }
}