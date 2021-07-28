/// \file

#ifndef CS3P01_PROYECTO_TSP_TSP_H
#define CS3P01_PROYECTO_TSP_TSP_H

#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "dbg.h"

namespace tsp {
template <typename W>
struct DenseGraph {
    std::vector<std::vector<W>> g;
    W const INF;
    DenseGraph(int n, W INF = std::numeric_limits<W>::max())
        : g(n, std::vector<W>(n, INF)), INF(INF) {}
    void add_edge(int u, int v, W w) { g[u][v] = w; }
    int size() const { return g.size(); }
    bool valid(int u, int v) { return g[u][v] != INF; }
    std::pair<DenseGraph<W>, std::vector<int>> subgraph(std::vector<int> nodes) {
        int n = g.size(), m = nodes.size();
        DenseGraph<W> h(m);
        std::vector<int> id(n, -1);
        for (int i = 0; i < m; ++i) id[nodes[i]] = i;
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (id[u] != -1 and id[v] != -1) h.add_edge(id[u], id[v], g[u][v]);
        return {h, id};
    }
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
template <typename W, typename BinOp = std::plus<W>>
std::pair<std::vector<int>, W> tsp(DenseGraph<W> g, int src, int dst, BinOp add = std::plus<W>()) {
    int n = g.size();
    std::vector<int> best_order;
    W best_cost = g.INF;

    std::function<void(Node<W>*)> bb = [&](Node<W>* u) {
        for (int v = 0; v < n; ++v) {
            if (u->vis & (1 << v)) continue;
            if (v == dst and __builtin_popcount(u->vis) + 1 != n) continue;
            if (not g.valid(u->id, v)) continue;
            if (add(u->cost, g[u->id][v]) >= best_cost) continue;
            bb(new Node<W>(v, u->vis | (1 << v), add(u->cost, g[u->id][v]), u));
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

    bb(new Node<W>(src, (1 << src), W(), nullptr));
    return {best_order, best_cost};
}
}  // namespace sequential

namespace parallel {
template <int P, typename W, typename BinOp = std::plus<W>>
std::pair<std::vector<int>, W> tsp(DenseGraph<W> g, int src, int dst, BinOp add = std::plus<W>()) {
    int n = g.size();
    std::vector<int> best_order;
    W best_cost = g.INF;
    auto threshold = [n](int level) { return (level - 1) * n > omp_get_num_threads(); };

    std::function<void(Node<W>*)> bb = [&](Node<W>* u) {
        int level = __builtin_popcount(u->vis);
        for (int v = 0; v < n; ++v) {
            if (u->vis & (1 << v)) continue;
            if (v == dst and level + 1 != n) continue;
            if (not g.valid(u->id, v)) continue;
            if (add(u->cost, g[u->id][v]) >= best_cost) continue;
#pragma omp task default(none) shared(g, bb, u, best_order, best_cost, add, n, src, dst) \
    firstprivate(v, level) final(threshold(level))
            bb(new Node<W>(v, u->vis | (1 << v), add(u->cost, g[u->id][v]), u));
        }

#pragma omp taskwait
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

#pragma omp parallel default(none) shared(g, bb, best_order, best_cost, add) \
    firstprivate(n, src, dst) num_threads(P)
#pragma omp single nowait
    { bb(new Node<W>(src, (1 << src), W(), nullptr)); }
    return {best_order, best_cost};
}
}  // namespace parallel

namespace parallel2 {
template <int P, typename W, typename BinOp = std::plus<W>>
std::pair<std::vector<int>, W> tsp(DenseGraph<W> g, int src, int dst, BinOp add = std::plus<W>()) {
    int n = g.size();
    std::vector<int> best_order;
    W best_cost = g.INF;
    auto threshold = [n](int level) { return (level - 1) * n > omp_get_num_threads(); };

    std::function<void(Node<W>*)> bb = [&](Node<W>* u) {
        int level = __builtin_popcount(u->vis);
        std::vector<int> candidates;
        for (int v = 0; v < n; ++v) {
            if (u->vis & (1 << v)) continue;
            if (v == dst and level + 1 != n) continue;
            if (not g.valid(u->id, v)) continue;
            if (add(u->cost, g[u->id][v]) >= best_cost) continue;
            candidates.push_back(v);
        }

#pragma omp taskloop default(none) shared(candidates, g, bb, u, best_order, best_cost, add, n, src, \
                                      dst) firstprivate(level) final(threshold(level)) nogroup
        for (int i = 0; i < (int)candidates.size(); ++i) {
            int v = candidates[i];
            bb(new Node<W>(v, u->vis | (1 << v), add(u->cost, g[u->id][v]), u));
        }

#pragma omp taskwait
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

#pragma omp parallel default(none) shared(g, bb, best_order, best_cost, add) \
    firstprivate(n, src, dst) num_threads(P)
#pragma omp single nowait
    { bb(new Node<W>(src, (1 << src), W(), nullptr)); }
    return {best_order, best_cost};
}
}  // namespace parallel2

}  // namespace tsp

#endif  // CS3P01_PROYECTO_TSP_TSP_H
