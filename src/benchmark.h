#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

void runBenchmarks(std::vector<long long>& latenciesNs);
void runMatchIterationStats(const std::vector<uint32_t>& matchIterations, const std::vector<long long>& latenciesNs);