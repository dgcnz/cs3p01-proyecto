#include <vector>

#include "catch2/catch.hpp"
#include "tsp/tsp.hpp"

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

    REQUIRE(order_seq == std::vector<int>{0, 1, 3, 2, 4});
    REQUIRE(cost_seq == 2);
}

TEST_CASE("Sequential vs Parallel TSP") {
    DenseGraph<int> g(5);

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(3, 2, 0);
    g.add_edge(2, 4, 1);
    g.add_edge(3, 4, 1);

    auto [order_seq, cost_seq] = sequential::tsp(g, 0, 4);
    auto [order_par, cost_par] = parallel::tsp(g, 0, 4);

    REQUIRE(order_seq == order_par);
    REQUIRE(cost_seq == cost_par);
}
