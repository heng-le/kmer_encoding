#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>

// Benchmark for vector push_back
static void BM_VectorPushBack(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> vec;
        for (int i = 0; i < state.range(0); ++i) {
            vec.push_back(i);
        }
    }
}
BENCHMARK(BM_VectorPushBack)->Range(8, 8 << 10);

// Benchmark for vector sort
static void BM_VectorSort(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    for (auto _ : state) {
        std::sort(data.begin(), data.end());
    }
}
BENCHMARK(BM_VectorSort)->Range(8, 8 << 10);

// Entry point for Google Benchmark
BENCHMARK_MAIN();
