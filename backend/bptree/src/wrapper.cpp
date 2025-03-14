#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bptree.h"

namespace py = pybind11;

PYBIND11_MODULE(_bptree, m) {
    m.doc() = "B+ tree module";

    py::class_<IntBPTree>(m, "IntBPTree")
        .def(py::init<int, const std::string&>(), py::arg("t"), py::arg("filename"))
        .def("insert", &IntBPTree::insert, py::arg("key"))
        .def("search", &IntBPTree::search, py::arg("key"))
        .def("save", &IntBPTree::save)
        .def("load", &IntBPTree::load);

    py::class_<StringBPTree>(m, "StringBPTree")
        .def(py::init<int, const std::string&>(), py::arg("t"), py::arg("filename"))
        .def("insert", &StringBPTree::insert, py::arg("key"))
        .def("search", &StringBPTree::search, py::arg("key"))
        .def("save", &StringBPTree::save)
        .def("load", &StringBPTree::load);
}
