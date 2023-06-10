project("CMake-eigen")

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

find_package(Eigen3)

add_executable(cmake_eigen src/pca_test.cpp)
target_link_libraries(cmake_eigen Eigen3::Eigen)
