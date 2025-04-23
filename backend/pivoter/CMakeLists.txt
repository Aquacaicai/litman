cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(GMP_INCLUDE_DIR "/usr/include" CACHE PATH "Path to GMP include directory")
set(GMP_LIBRARY "/usr/lib/libgmp.so" CACHE PATH "Path to GMP library")

if(NOT EXISTS "${GMP_INCLUDE_DIR}/gmp.h")
    message(FATAL_ERROR "GMP headers not found. Set GMP_INCLUDE_DIR to the directory containing gmp.h")
endif()

if(NOT EXISTS "${GMP_LIBRARY}")
    message(FATAL_ERROR "GMP library not found. Set GMP_LIBRARY to the path of libgmp.so")
endif()

message(STATUS "Using GMP include dir: ${GMP_INCLUDE_DIR}")
message(STATUS "Using GMP library: ${GMP_LIBRARY}")

set(GMP_LIBRARIES ${GMP_LIBRARY})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_pivoter MODULE
    src/pivoter.h
    src/neighbor_list.h
    src/misc.h
    src/wrapper.cpp
)

target_link_libraries(_pivoter PRIVATE ${GMP_LIBRARIES})
target_include_directories(_pivoter PRIVATE ${GMP_INCLUDE_DIRS})

install(TARGETS _pivoter DESTINATION ${SKBUILD_PROJECT_NAME})
