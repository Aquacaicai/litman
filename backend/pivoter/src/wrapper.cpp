#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pivoter.h"

namespace py = pybind11;

std::map<int, std::string> pivoter(const std::vector<std::set<int>>& py_adjacency_list) {
    int n = py_adjacency_list.size();
    std::list<int>** adjList = new std::list<int>*[n];

    for (int i = 0; i < n; i++) {
        adjList[i] = new std::list<int>();
        for (const auto& neighbor : py_adjacency_list[i]) {
            adjList[i]->push_back(neighbor);
        }
    }

    int deg = 0;
    NeighborListArray** orderingArray = computeDegeneracyOrderArray(adjList, n);

    for (int i = 0; i < n; i++) {
        if (deg < orderingArray[i]->laterDegree) deg = orderingArray[i]->laterDegree;
    }

    mpz_t* cliqueCounts = new mpz_t[deg + 1];
    for (int i = 0; i <= deg; i++) {
        mpz_init(cliqueCounts[i]);
        mpz_set_ui(cliqueCounts[i], 0);
    }

    listAllCliquesDegeneracy_A(cliqueCounts, orderingArray, n, deg);

    std::map<int, std::string> result;
    char buffer[1024];

    for (int i = 0; i <= deg; i++) {
        if (mpz_cmp_ui(cliqueCounts[i], 0) != 0) {
            gmp_snprintf(buffer, sizeof(buffer), "%Zd", cliqueCounts[i]);
            result[i] = std::string(buffer);
        }
    }

    for (int i = 0; i <= deg; i++) {
        mpz_clear(cliqueCounts[i]);
    }
    delete[] cliqueCounts;

    for (int i = 0; i < n; i++) {
        delete adjList[i];
    }
    delete[] adjList;

    return result;
}
PYBIND11_MODULE(_pivoter, m) {
    m.def("pivoter", &pivoter, "Calculate clique counts for each degree");
}
