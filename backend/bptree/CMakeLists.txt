cmake_minimum_required(VERSION 3.15)
project(bptree)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

pybind11_add_module(_bptree
    src/bptree.cpp
    src/wrapper.cpp
)

target_include_directories(_bptree PRIVATE ${pybind11_INCLUDE_DIRS})
