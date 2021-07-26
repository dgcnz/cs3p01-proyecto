/// \file

#ifndef CS3P01_PROYECTO_TSP_TSP_H
#define CS3P01_PROYECTO_TSP_TSP_H

#include <functional>
#include <limits>
#include <utility>
#include <vector>

namespace tsp {
template <typename W, W INF = std::numeric_limits<W>::max()>
struct DenseGraph {
    std::vector<std::vector<W>> g;
    DenseGraph(int n) : g(n, std::vector<int>(n, INF)) {}
    void add_edge(int u, int v, W w) { g[u][v] = w; }
    int size() const { return g.size(); }
    std::vector<W>& operator[](int u) { return g[u]; }
    std::vector<W> operator[](int u) const { return g[u]; }
};

template <typename W, W INF = std::numeric_limits<W>::max()>
std::pair<std::vector<int>, W> tsp(DenseGraph<W, INF>& g, int src, int dst) {
    int n = g.size();
    std::vector<bool> vis(n, false);
    W best_cost = INF;
    W cost = W();
    std::vector<int> order = {src};
    std::vector<int> best_order;

    std::function<void(int)> bb = [&](int u) {
        vis[u] = true;
        for (int v = 0; v < n; ++v) {
            if (g[u][v] == INF or vis[v] or (v == dst and (int) order.size() + 1 != n)) continue;

            if (cost + g[u][v] < best_cost) {
                order.push_back(v);
                cost += g[u][v];
                bb(v);
                order.pop_back();
                cost -= g[u][v];
            }
        }
        if ((int)order.size() == n) {
            best_cost = cost;
            best_order = order;
        }
        vis[u] = false;
    };

    bb(src);
    return {best_order, best_cost};
}

}  // namespace tsp

#endif  // CS3P01_PROYECTO_TSP_TSP_H
