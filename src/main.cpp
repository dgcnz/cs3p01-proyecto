/// \file
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "dbg.h"
#include "httplib/httplib.h"
#include "tsp/tsp.hpp"

/*
0: Lima Centro
1: Lince
2: Miraflores
3: Barranco
4. Rimac
5. Los Olivos
6. La Molina
7. La Victoria
8. Magdalena
9. San Borja
*/
const int N = 10;
const double distance_matrix[N][N] = {
    {0, 5.1, 9.3, 11.8, 3.7, 12.5, 17.9, 6.5, 6.4, 10.2},
    {5.0, 0, 3.2, 7.0, 7.7, 17.5, 13.7, 2.9, 5.2, 5.8},
    {9.0, 3.8, 0, 4.0, 11.7, 21.5, 13.9, 5.8, 6.8, 6.2},
    {12.3, 7.9, 4.2, 0, 15.0, 24.8, 16.7, 9.4, 11.5, 9.5},
    {4.6, 7.8, 11.9, 14.3, 0, 12.1, 18.7, 8.1, 9.9, 12.9},
    {13.7, 17.1, 21.4, 23.8, 12.3, 0, 29.8, 17.9, 17.1, 22.3},
    {17.5, 14.0, 14.7, 17.7, 19.2, 28.9, 0, 12.6, 17.3, 9.3},
    {6.0, 3.5, 6.2, 9.2, 8.0, 17.7, 13.0, 0, 7.0, 4.7},
    {6.5, 5.0, 6.0, 9.1, 10.2, 16.7, 17.2, 7.4, 0, 9.5},
    {9.6, 6.3, 5.5, 8.8, 12.6, 21.9, 9.2, 4.6, 9.6, 0},
};

/// Program entry point.
int main(void) {
    httplib::Server svr;

    dbg(svr.set_mount_point("/", "../www"));

    tsp::DenseGraph<double> g(N);
    for (int u = 0; u < N; ++u)
        for (int v = 0; v < N; ++v) g.add_edge(u, v, distance_matrix[u][v]);

    svr.Get("/tsp", [&g](const httplib::Request& req, httplib::Response& res) {
        /* Input */
        std::vector<int> v;
        std::istringstream is(req.get_param_value("v"));
        std::string str;
        while (std::getline(is, str, ',')) v.push_back(std::stoi(str));
        int src = std::stoi(req.get_param_value("src")),
            dst = std::stoi(req.get_param_value("dst"));
        dbg(v);
        dbg(src);
        dbg(dst);

        /* Parallel tsp */
        auto [h, id] = g.subgraph(v);
        auto [order, cost] = tsp::parallel::tsp<2>(h, id[src], id[dst]);

        for (int i = 0; i < (int)order.size(); ++i) {
            order[i] = v[order[i]];
        }

        /* Output */
        std::ostringstream os;
        os << "{ \"order\": [";
        std::copy(order.begin(), order.end(), std::ostream_iterator<int>(os, ","));
        if (!order.empty()) os.seekp(-1, os.cur);
        os << "], \"cost\": " << cost << " }";
        std::string content(os.str());

        res.set_content(content, "application/json");
    });

    svr.Get("/stop", [&](const httplib::Request&, httplib::Response&) { svr.stop(); });

    svr.listen("0.0.0.0", 8080);

    return EXIT_SUCCESS;
}
