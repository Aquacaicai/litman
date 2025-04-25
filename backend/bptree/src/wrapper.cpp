#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;


#include "bptree.h"
PYBIND11_MODULE(_bptree, m) {

    py::class_<BPTree<int, std::string>>(m, "BPTreeIntStr")
        .def(py::init<int>())
        .def("insert", &BPTree<int, std::string>::insert)
        .def("update", &BPTree<int, std::string>::update)
        .def("find", &BPTree<int, std::string>::find)
        .def("deserialize", &BPTree<int, std::string>::deserialize)
        .def("serialize", &BPTree<int, std::string>::serialize)
        .def("keys", &BPTree<int, std::string>::keys)
        .def("values", &BPTree<int, std::string>::values);

    py::class_<BPTree<int, std::vector<int>>>(m, "BPTreeIntVecInt")
        .def(py::init<int>())
        .def("insert", &BPTree<int, std::vector<int>>::insert)
        .def("update", &BPTree<int, std::vector<int>>::update)
        .def("find", &BPTree<int, std::vector<int>>::find)
        .def("deserialize", &BPTree<int, std::vector<int>>::deserialize)
        .def("serialize", &BPTree<int, std::vector<int>>::serialize)
        .def("keys", &BPTree<int, std::vector<int>>::keys)
        .def("values", &BPTree<int, std::vector<int>>::values);

    py::class_<BPTree<std::wstring, int>>(m, "BPTreeWStrInt")
        .def(py::init<int>())
        .def("insert", &BPTree<std::wstring, int>::insert)
        .def("update", &BPTree<std::wstring, int>::update)
        .def("find", &BPTree<std::wstring, int>::find)
        .def("deserialize", &BPTree<std::wstring, int>::deserialize)
        .def("serialize", &BPTree<std::wstring, int>::serialize)
        .def("keys", &BPTree<std::wstring, int>::keys)
        .def("values", &BPTree<std::wstring, int>::values);

    py::class_<BPTree<std::wstring, std::vector<int>>>(m, "BPTreeWStrVecInt")
        .def(py::init<int>())
        .def("insert", &BPTree<std::wstring, std::vector<int>>::insert)
        .def("update", &BPTree<std::wstring, std::vector<int>>::update)
        .def("find", &BPTree<std::wstring, std::vector<int>>::find)
        .def("deserialize", &BPTree<std::wstring, std::vector<int>>::deserialize)
        .def("serialize", &BPTree<std::wstring, std::vector<int>>::serialize)
        .def("keys", &BPTree<std::wstring, std::vector<int>>::keys)
        .def("values", &BPTree<std::wstring, std::vector<int>>::values);
}
