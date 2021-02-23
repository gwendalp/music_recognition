cmake_minimum_required(VERSION 3.16)
project(start_svm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER gcc-10.2)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories(include)

add_executable(extraction extraction.cpp)
add_executable(predict predict.cpp)
target_link_libraries(extraction tbb)
target_link_libraries(predict tbb Eigen3::Eigen)


