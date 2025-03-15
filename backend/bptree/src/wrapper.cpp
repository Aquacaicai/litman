#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

#include "bptree.h"

PYBIND11_MODULE(_bptree, m) {
    py::class_<BPTree<int, int>>(m, "BPTreeIntInt")
        .def(py::init<int>())
        .def("insert", &BPTree<int, int>::insert)
        .def("erase", &BPTree<int, int>::erase)
        .def("update", &BPTree<int, int>::update)
        .def("find", &BPTree<int, int>::find)
        .def("deserialize", &BPTree<int, int>::deserialize)
        .def("serialize", &BPTree<int, int>::serialize);

    py::class_<BPTree<int, std::string>>(m, "BPTreeIntStr")
        .def(py::init<int>())
        .def("insert", &BPTree<int, std::string>::insert)
        .def("erase", &BPTree<int, std::string>::erase)
        .def("update", &BPTree<int, std::string>::update)
        .def("find", &BPTree<int, std::string>::find)
        .def("deserialize", &BPTree<int, std::string>::deserialize)
        .def("serialize", &BPTree<int, std::string>::serialize);

    py::class_<BPTree<int, std::vector<int>>>(m, "BPTreeIntVecInt")
        .def(py::init<int>())
        .def("insert", &BPTree<int, std::vector<int>>::insert)
        .def("erase", &BPTree<int, std::vector<int>>::erase)
        .def("update", &BPTree<int, std::vector<int>>::update)
        .def("find", &BPTree<int, std::vector<int>>::find)
        .def("deserialize", &BPTree<int, std::vector<int>>::deserialize)
        .def("serialize", &BPTree<int, std::vector<int>>::serialize);

    py::class_<BPTree<std::string, int>>(m, "BPTreeStrInt")
        .def(py::init<int>())
        .def("insert", &BPTree<std::string, int>::insert)
        .def("erase", &BPTree<std::string, int>::erase)
        .def("update", &BPTree<std::string, int>::update)
        .def("find", &BPTree<std::string, int>::find)
        .def("deserialize", &BPTree<std::string, int>::deserialize)
        .def("serialize", &BPTree<std::string, int>::serialize);

    py::class_<BPTree<std::string, std::string>>(m, "BPTreeStrStr")
        .def(py::init<int>())
        .def("insert", &BPTree<std::string, std::string>::insert)
        .def("erase", &BPTree<std::string, std::string>::erase)
        .def("update", &BPTree<std::string, std::string>::update)
        .def("find", &BPTree<std::string, std::string>::find)
        .def("deserialize", &BPTree<std::string, std::string>::deserialize)
        .def("serialize", &BPTree<std::string, std::string>::serialize);

    py::class_<BPTree<std::string, std::vector<int>>>(m, "BPTreeStrVecInt")
        .def(py::init<int>())
        .def("insert", &BPTree<std::string, std::vector<int>>::insert)
        .def("erase", &BPTree<std::string, std::vector<int>>::erase)
        .def("update", &BPTree<std::string, std::vector<int>>::update)
        .def("find", &BPTree<std::string, std::vector<int>>::find)
        .def("deserialize", &BPTree<std::string, std::vector<int>>::deserialize)
        .def("serialize", &BPTree<std::string, std::vector<int>>::serialize);
}