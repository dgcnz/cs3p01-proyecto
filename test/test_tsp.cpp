#include <utility>
#include <vector>

#include "catch2/catch.hpp"
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
    DenseGraph<int> g(5);

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 0);
    g.add_edge(3, 2, 0);
    g.add_edge(2, 4, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);

    auto [order_seq, cost_seq] = sequential::tsp(g, 0, 4);
    auto [order_par, cost_par] = parallel::tsp(g, 0, 4);

    REQUIRE(order_seq == order_par);
    REQUIRE(cost_seq == cost_par);
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
    auto [order_par, cost_par] = parallel::tsp(g, 0, 4, add_pair);

    REQUIRE(order_seq == vector<int>{0, 1, 3, 2, 4});
    REQUIRE(cost_seq == ii{2, 0});
}
