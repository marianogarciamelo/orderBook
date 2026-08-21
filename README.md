# orderBook

A C++17 limit order book / matching engine, built as a low-latency systems learning project. Processes buy/sell orders against a price-time priority book and produces nanosecond-resolution latency benchmarks over 1M+ synthetic orders.

## What it does

- Maintains a two-sided limit order book (`bids` / `asks`) keyed by integer price (ticks/cents — floats are never used as map keys)
- Supports `ADD`, `CANCEL`, and matching (`EXECUTE`) semantics: every incoming order attempts to match against the opposite side before resting, mirroring real exchange behavior
- Assigns and owns internal order IDs, with a separate mapping layer to bridge external (CSV-supplied) IDs to internal ones
- Parses order flow from CSV and replays it through the book
- Benchmarks `addOrder`, `cancelOrder`, and `executeOrder` with `std::chrono::high_resolution_clock`, reporting mean/median/p95/p99/max latency and match-depth statistics

## Architecture

```
src/
  Order.h            # Order struct, ActionType enum
  OrderBook.h/.cpp    # Core book: add / cancel / execute (matching engine)
  PooledList.h        # Custom memory pool (Node, NodePool, PriceLevelList)
  CsvParser.h/.cpp     # CSV -> CsvEvent parsing and replay
  benchmark.h/.cpp     # Latency measurement and percentile reporting
  generateData.cpp     # Synthetic order flow generator (fixed seed, reproducible)
data/                  # Generated CSV order flow
build/                 # CMake build artifacts
```

- **Price levels**: `std::map<int, PriceLevelList>` — sorted so best bid/ask is always `begin()`, at the cost of red-black tree insert/erase on price-level churn (see Results below).
- **Order storage per price level**: originally `std::list<Order>`; replaced with a custom `PriceLevelList` backed by a shared `NodePool` — a pre-allocated, fixed-capacity vector of nodes with free-list index recycling. This gives O(1) allocate/free with no per-order heap allocation on the hot path.
- **ID lookups**: `std::unordered_map<uint64_t, Node*>` per side, so `cancelOrder` is O(1) average-case direct-to-node.

## Engineering process & key decisions

This project was built as a deliberate latency-debugging exercise, not just a working matching engine:

1. **Baseline benchmark** surfaced a ~4,000x outlier (59ms max against a low-µs median) — investigated rather than ignored.
2. **Ruled out match depth** as the cause by instrumenting every operation with a `matchIterations` count and correlating it against latency — the slowest operations consistently had 0–1 match iterations, meaning depth of matching wasn't the driver.
3. **Found `std::cout` calls live inside the timed hot path** (`executeOrder`, `cancelOrder`) — a classic case of debug instrumentation silently dominating a "clean" benchmark. Removing them was the single biggest latency win.
4. **Diagnosed remaining tail latency to `std::list`'s per-node heap allocation** (`new` on every `push_back`) and replaced it with a custom memory pool, dropping mean and median latency.
5. **Investigated `std::map`'s own tree-node churn** on price-level insert/erase as the next suspect. Tried skipping erasure of empty price levels to avoid repeated tree rebalancing — reverted after this forced best-bid/ask lookups to walk over dead (empty) price levels, trading allocation churn for scan overhead. Net conclusion: the remaining tail is a genuine architectural tradeoff of `std::map`'s sorted-order guarantee, not a bug to patch away.

## Results

Benchmarked across two very different environments, 1,000,000 synthetic orders each:

| Metric | AWS EC2 (free-tier, shared tenancy) | M4 MacBook Pro (dedicated, idle) |
|---|---|---|
| Mean | 980.8 ns | 303.7 ns |
| Median | 1,077 ns | 292 ns |
| p95 | 1,618 ns | 584 ns |
| p99 | 2,694 ns | 958 ns |
| Max | 212,576 ns | 32,709 ns |
| Fill rate | 49.99% | 49.99% |
| Rest rate | 50.03% | 50.03% |

Implied throughput: **~1.02M ops/sec** (EC2) vs. **~3.29M ops/sec** (M4).

## Building

```bash
mkdir build && cd build
cmake ..
make
./orderBook
```

CI runs on GitHub Actions (GCC, Linux) to catch compiler-specific issues (e.g. missing explicit includes that happen to compile via transitive includes locally).

## What I'd do next

- Custom allocator for `std::map`'s internal tree nodes (or an alternative sorted-price structure) to close out the last tail-latency source
- Multi-threaded order ingestion with lock-free or sharded structures
- Persist/replay book state for crash recovery
