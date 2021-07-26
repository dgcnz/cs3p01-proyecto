#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

using namespace std;

template <typename W, W INF = numeric_limits<W>::max()>
struct DenseGraph {
    vector<vector<W>> g;
    DenseGraph(int n) : g(n, vector<int>(n, INF)) {}
    void add_edge(int u, int v, W w) { g[u][v] = w; }
    size_t size() const { return g.size(); }
    vector<W>& operator[](int u) { return g[u]; }
    vector<W> operator[](int u) const { return g[u]; }
};

template <typename W, W INF = numeric_limits<W>::max()>
pair<vector<int>, W> tsp(DenseGraph<W, INF>& g, int src, int dst) {
    int n = g.size();
    vector<bool> vis(n, false);
    W best_cost = INF;
    W cost = W();
    vector<int> order = {src};
    vector<int> best_order;

    function<void(int)> bb = [&](int u) {
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

// int main(void)
// {
//
//     DenseGraph<int> g(5);
//     g.add_edge(0, 1, 1);
//     g.add_edge(1, 2, 1);
//     g.add_edge(1, 3, 0);
//     g.add_edge(3, 2, 0);
//     g.add_edge(2, 4, 1);
//     g.add_edge(2, 3, 1);
//     g.add_edge(3, 4, 1);
//     auto [order, cost] = tsp(g, 0, 4);
//     return 0;
// }
