cmake_minimum_required(VERSION 3.16)
project(start_svm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER gcc-10.2)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories(include)

add_executable(output main.cpp)
add_executable(predict predict.cpp)
#add_executable(eigen eigen.cpp)


# target_link_libraries(predict mfcc tbb fftw3 Eigen3::Eigen)


#target_link_libraries(eigen tbb Eigen3::Eigen)

target_link_libraries(output tbb)
target_link_libraries(predict tbb Eigen3::Eigen)


