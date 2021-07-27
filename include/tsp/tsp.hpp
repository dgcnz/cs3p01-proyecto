/// \file

#ifndef CS3P01_PROYECTO_TSP_TSP_H
#define CS3P01_PROYECTO_TSP_TSP_H

#include <algorithm>
#include <cstdio>
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

template <typename W>
struct Node {
    int id;
    int vis;
    W cost;
    Node* parent;
    Node(int id, int vis, W cost, Node* parent) : id(id), vis(vis), cost(cost), parent(parent) {}
};

namespace sequential {
template <typename W, W INF = std::numeric_limits<W>::max()>
std::pair<std::vector<int>, W> tsp(DenseGraph<W, INF> g, int src, int dst) {
    int n = g.size();
    std::vector<int> best_order;
    W best_cost = INF;

    std::function<void(Node<W>*)> bb = [&](Node<W>* u) {
        for (int v = 0; v < n; ++v) {
            if (u->vis & (1 << v)) continue;
            if (v == dst and __builtin_popcount(u->vis) + 1 != n) continue;
            if (g[u->id][v] == INF) continue;
            if (u->cost + g[u->id][v] > best_cost) continue;
            bb(new Node<W>(v, u->vis | (1 << v), u->cost + g[u->id][v], u));
        }
        if (u->id == dst) {
            Node<W>* temp = u;
            best_order.clear();
            while (temp) {
                best_order.push_back(temp->id);
                temp = temp->parent;
            }
            best_cost = u->cost;
            std::reverse(best_order.begin(), best_order.end());
        }
        delete u;
    };

    bb(new Node<W>(src, (1 << src), 0, nullptr));
    return {best_order, best_cost};
}
}  // namespace sequential

namespace parallel {
template <typename W, W INF = std::numeric_limits<W>::max()>
std::pair<std::vector<int>, W> tsp(DenseGraph<W, INF> g, int src, int dst) {
    int n = g.size();
    std::vector<int> best_order;
    W best_cost = INF;

    std::function<void(Node<W>*)> bb = [&](Node<W>* u) {
#pragma omp taskloop default(none) shared(g, bb, u, best_order, best_cost) firstprivate(n, src, dst)
        for (int v = 0; v < n; ++v) {
            if (u->vis & (1 << v)) continue;
            if (v == dst and __builtin_popcount(u->vis) + 1 != n) continue;
            if (g[u->id][v] == INF) continue;
            if (u->cost + g[u->id][v] > best_cost) continue;
            bb(new Node<W>(v, u->vis | (1 << v), u->cost + g[u->id][v], u));
        }
        if (u->id == dst) {
            Node<W>* temp = u;
            std::vector<int> order;
            while (temp) {
                order.push_back(temp->id);
                temp = temp->parent;
            }
            reverse(order.begin(), order.end());
#pragma omp critical
            {
                if (u->cost < best_cost) {
                    best_cost = u->cost;
                    best_order = order;
                }
            }
        }
        delete u;
    };

#pragma omp parallel default(none) shared(g, bb, best_order, best_cost) firstprivate(n, src, dst)
#pragma omp single nowait
    { bb(new Node<W>(src, (1 << src), 0, nullptr)); }
    return {best_order, best_cost};
}
}  // namespace parallel

}  // namespace tsp

#endif  // CS3P01_PROYECTO_TSP_TSP_H
