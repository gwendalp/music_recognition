#ifndef START_SVM_GLOBAL_H
#define START_SVM_GLOBAL_H

#include <complex>
#include <vector>

typedef float real;
typedef std::complex<real> Complex;
typedef std::vector<real> DataVector;

constexpr std::size_t N = 512;
constexpr real Fs = 22050.0;

#endif //START_SVM_GLOBAL_H
