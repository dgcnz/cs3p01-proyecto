/// \file

#include <omp.h>
#include <stdio.h>

#include <cassert>
#include <iostream>

#include "tsp/tsp.hpp"

using namespace tsp;

/// Program entry point.
int main(const int argc, const char* argv[]) {
    DenseGraph<int> g(5);

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 0);
    g.add_edge(3, 2, 0);
    g.add_edge(2, 4, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);

    auto [order1, cost1] = sequential::tsp(g, 0, 4);
    auto [order2, cost2] = parallel::tsp(g, 0, 4);

    assert(order1 == order2);
    assert(cost1 == cost2);


    std::cout <<"COST: " << cost1 << std::endl;
    std::cout << "ORDER: ";
    for (auto x : order1) std::cout << x << " ";
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
