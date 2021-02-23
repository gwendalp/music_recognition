#ifndef START_SVM_SIGNAL_H
#define START_SVM_SIGNAL_H

#include <iostream>
#include <iterator>
#include <numbers>
#include <array>
#include <random>
#include <chrono>
#include <execution>
#include <type_traits>
#include <vector>

// Print vector
template<typename T>
std::ostream &operator<<(std::ostream &s, const std::vector<T> &v) {
    s << '[';
    if (!v.empty()) {
        std::copy(v.cbegin(), v.cend() - 1, std::ostream_iterator<T>(s, ", "));
        s << v.back();
    }
    return s << ']';
}



// FFT
constexpr std::array<Complex, N / 2> twiddle_factors() {
    std::array<Complex, N / 2> t;
    for (std::size_t k = 0; k < N / 2; k++) {
        t[k] = std::exp(std::complex<real>(0, -2 * std::numbers::pi * k / N));
    }
    return t;
}

constexpr std::array<std::size_t, N> bit_reverse_array() {
    std::array<std::size_t, N> unscrambled{};
    std::size_t m = std::log2(N);
    //std::size_t m = ((unsigned) (8 * sizeof(unsigned long long) - __builtin_clzll((N)) - 1));
    for (std::size_t i = 0; i < N; i++) {
        std::size_t j = i;
        j = (((j & 0xaaaaaaaa) >> 1) | ((j & 0x55555555) << 1));
        j = (((j & 0xcccccccc) >> 2) | ((j & 0x33333333) << 2));
        j = (((j & 0xf0f0f0f0) >> 4) | ((j & 0x0f0f0f0f) << 4));
        j = (((j & 0xff00ff00) >> 8) | ((j & 0x00ff00ff) << 8));
        j = ((j >> 16) | (j << 16)) >> (32 - m);
        if (i < j) {
            unscrambled[i] = j;
        } else
            unscrambled[i] = i;
    }
    return unscrambled;
}


void ite_dit_fft(std::vector<Complex> &x) {
    std::size_t problemSize = x.size();
    std::size_t stages = std::log2(problemSize);
    auto tf = twiddle_factors();

    constexpr std::array<std::size_t, N> unscrambled = bit_reverse_array();
    for (std::size_t i = 0; i < x.size(); i++) {
        std::size_t j = unscrambled[i];
        if (i < j) {
            swap(x[i], x[j]);
        }
    }

    for (std::size_t stage = 0; stage <= stages; stage++) {
        std::size_t currentSize = 1 << stage;
        std::size_t step = stages - stage;
        std::size_t halfSize = currentSize / 2;
        for (std::size_t k = 0; k < problemSize; k = k + currentSize) {
            //for (std::size_t k = 0; k <= problemSize - currentSize; k = k + currentSize) {
            for (std::size_t j = 0; j < halfSize; j++) {
                auto u = x[k + j];
                auto v = x[k + j + halfSize] * tf[j * (1 << step)];
                x[k + j] = (u + v);
                x[k + j + halfSize] = (u - v);
            }
        }
    }
}

constexpr std::array<real, N> rect_window() {
    std::array<real, N> w;
    std::fill(w.begin(), w.end(), 1.0);
    return w;
}

constexpr std::array<real, N> hamming_window() {
    std::array<real, N> w;
    std::generate(w.begin(), w.end(),
                  [&, index = -1]()mutable {
                      index++;
                      return (0.54 - 2 * 0.23 * std::cos(2 * std::numbers::pi * index / N));
                  });
    return w;
}

template<typename T>
std::vector<T> generate_random_vector(real inf, real sup, std::size_t size) {
    std::vector<T> a(size);
    std::random_device seeder;
    std::mt19937 random_generator(seeder()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> uniformRealDistribution(inf, sup);
    std::generate(a.begin(), a.end(), [&]() { return uniformRealDistribution(random_generator); });
    return a;
}

DataVector toReal(const std::vector<Complex> &x) {
    DataVector r(x.size());
    std::transform(x.begin(), x.end(), r.begin(), [](Complex c) { return c.real(); });
    return r;
}

DataVector toImag(const std::vector<Complex> &x) {
    DataVector r(x.size());
    std::transform(x.begin(), x.end(), r.begin(), [](Complex c) { return c.imag(); });
    return r;
}

std::vector<Complex> toComplex(const DataVector &x) {
    std::vector<Complex> r(x.size());
    std::generate(r.begin(), r.end(), [&, k = -1]()mutable {
        k++;
        return Complex(x[k], 0.0);
    });
    return r;
}

std::vector<Complex> toComplex(const DataVector &r, const DataVector &i) {
    std::vector<Complex> c(r.size());
    std::generate(c.begin(), c.end(), [&, k = -1]()mutable {
        k++;
        return Complex(r[k], i[k]);
    });
    return c;
}

void windowing(const std::array<real, N> &w, std::vector<Complex> &s) {
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [&, index = -1](Complex c)mutable {
                       index++;
                       return w[index] * c;
                   });
}

template<typename T>
std::pair<real, real> avgstddev(std::vector<T> &data) {
    auto avg = std::reduce(data.cbegin(), data.cend()) / data.size();
    auto stddev = std::sqrt(std::transform_reduce(data.cbegin(),
                                                  data.cend(),
                                                  0.0,
                                                  std::plus<real>(),
                                                  [&avg](T d) -> real { return (d - avg) * (d - avg); }) /
                            (data.size() - 1));
    return std::make_pair(avg, stddev);
}

DataVector psd(const std::vector<Complex> &dft) {
    DataVector p(dft.size());
    for (std::size_t k = 0; k < p.size(); k++)
        p[k] = (std::norm(dft[k]));
    return p;
}

std::vector<std::vector<Complex>> stft(const DataVector &signal) {
    std::size_t k = 0;
    std::size_t n_chunks = signal.size() / N;
    std::vector<std::vector<Complex>> bins;
    auto w = hamming_window();
    for (std::size_t k = 0; k < signal.size() / N; k++) {
        std::vector<Complex> v1(N);
        std::vector<Complex> v2(N);
        std::copy(signal.cbegin() + k * N, signal.cbegin() + k * N + N, v1.begin()); // data[k * N:k * N + N];
        std::copy(signal.cbegin() + N / 2 + k * N, signal.cbegin() + k * N + N / 2 + N, v2.begin());
        windowing(w, v1);
        windowing(w, v2);
        ite_dit_fft(v1);
        ite_dit_fft(v2);
        bins.push_back(v1);
        bins.push_back(v2);
    }
    bins.shrink_to_fit();
    return bins;
}

DataVector binsavgstd(std::vector<std::vector<Complex>> &bins) {
    auto w = hamming_window();
    auto ww = std::reduce(w.cbegin(), w.cend(), 0.0, std::plus<real>());
    auto scale = 1 / (ww);
    DataVector m(N/2);
    DataVector s(N / 2);
    DataVector m_prev(N / 2);
    std::size_t n = 0;
    for (auto v: bins) {
        n++;
        for (std::size_t k = 0; k < N / 2; ++k) {
            auto x = std::abs(v[k]) * scale;
            m_prev[k] = m[k];
            m[k] += (x - m[k]) / n;
            s[k] += (x - m[k]) * (x - m_prev[k]);
        }
    }
    for (std::size_t k = 0; k < s.size(); k++) {
        s[k] = std::sqrt(s[k] / (n - 1));
    }
    m.insert(m.end(), s.begin(), s.end());
    return m;
}


#endif //START_SVM_SIGNAL_H
