/// \file

#include <iostream>

#include "tsp/tsp.h"

/// Program entry point.
int main(const int argc, const char* argv[]) {
    tsp::DenseGraph<int> g(5);

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 0);
    g.add_edge(3, 2, 0);
    g.add_edge(2, 4, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);

    auto [order, cost] = tsp::tsp(g, 0, 4);

    std::cout << cost << std::endl;

    return EXIT_SUCCESS;
}
