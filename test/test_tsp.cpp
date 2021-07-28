#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <utility>
#include <vector>

#include "catch2/catch.hpp"
#include "dbg.h"
#include "tsp/tsp.hpp"
using namespace std;
using namespace tsp;

TEST_CASE("Sequential TSP") {
    DenseGraph<int> g(5);

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 0);
    g.add_edge(3, 2, 0);
    g.add_edge(2, 4, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);

    auto [order_seq, cost_seq] = sequential::tsp(g, 0, 4);
    REQUIRE(order_seq == vector<int>{0, 1, 3, 2, 4});
    REQUIRE(cost_seq == 2);
}

TEST_CASE("Sequential vs Parallel TSP") {
    auto generate = [](int n) {
        srand(time(NULL));
        DenseGraph<int> g(n);
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (u != v) g.add_edge(u, v, abs(rand()) % 100 + 1);
        return g;
    };

    for (int t = 0; t < 20; ++t) {
        DenseGraph<int> g = generate(10);
        auto [order_seq, cost_seq] = sequential::tsp(g, 0, 1);
        auto [order_par, cost_par] = parallel::tsp<4>(g, 0, 1);
        REQUIRE(cost_seq == cost_par);
        // REQUIRE(order_seq == order_par);
    }
}

TEST_CASE("Sequential multiple parameter edges") {
    using ii = pair<int, int>;
    ii INF = make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());

    auto add_pair = [](ii a, ii b) { return make_pair(a.first + b.first, a.second + b.second); };

    DenseGraph<ii> g(5, INF);

    g.add_edge(0, 1, ii{1, 0});
    g.add_edge(1, 2, ii{1, 0});
    g.add_edge(1, 3, ii{0, 0});
    g.add_edge(3, 2, ii{0, 0});
    g.add_edge(2, 4, ii{1, 0});
    g.add_edge(2, 3, ii{1, 0});
    g.add_edge(3, 4, ii{1, 0});

    auto [order_seq, cost_seq] = sequential::tsp(g, 0, 4, add_pair);
    auto [order_par, cost_par] = parallel::tsp<4>(g, 0, 4, add_pair);

    REQUIRE(cost_seq == ii{2, 0});
    CHECK(order_seq == vector<int>{0, 1, 3, 2, 4});
}

TEST_CASE("Benchmarks") {
    auto generate = [](int n) {
        srand(time(NULL));
        DenseGraph<int> g(n);
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (u != v) g.add_edge(u, v, abs(rand()) % 100 + 1);
        return g;
    };

    for (int n = 10; n < 15; ++n) {
        BENCHMARK("Sequential " + to_string(n)) { return sequential::tsp(generate(n), 0, 1); };
        BENCHMARK("Parallel " + to_string(n)) { return parallel::tsp<4>(generate(n), 0, 1); };
    }
}
