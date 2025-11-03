#include <iostream>
#include <vector>
#include <deque>
#include <chrono>
#include <iomanip>
#include <climits>
#include <cerrno>
#include "FordJohnson.hpp"

static bool parsePositiveInt(const char* s, int &out) {
    errno = 0;
    char *end = nullptr;
    const long val = std::strtol(s, &end, 10);
    if (errno == ERANGE || end == s || *end != '\0') return false;
    if (val <= 0 || val > INT_MAX) return false;
    out = static_cast<int>(val);
    return true;
}

int main(const int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Error: provide a sequence of positive integers as arguments." << std::endl;
        return 1;
    }

    std::vector<int> inputVec; inputVec.reserve(static_cast<size_t>(argc - 1));
    std::deque<int>  inputDeq;

    for (int i = 1; i < argc; ++i) {
        int v;
        if (!parsePositiveInt(argv[i], v)) {
            std::cerr << "Error" << std::endl;
            return 1;
        }
        inputVec.push_back(v);
        inputDeq.push_back(v);
    }

    std::cout << "Before:  ";
    for (size_t i = 0; i < inputVec.size(); ++i) { if (i) std::cout << ' '; std::cout << inputVec[i]; }
    std::cout << std::endl;

    FordJohnson<std::vector<int>> sorterVec;
    FordJohnson<std::deque<int>>  sorterDeq;

    const auto t0v = std::chrono::high_resolution_clock::now();
    const std::vector<int> vecSorted = sorterVec.sort(inputVec);
    const auto t1v = std::chrono::high_resolution_clock::now();
    const auto durVecUs = std::chrono::duration_cast<std::chrono::microseconds>(t1v - t0v).count();

    const auto t0d = std::chrono::high_resolution_clock::now();
    const std::deque<int> deqSorted = sorterDeq.sort(inputDeq);
    const auto t1d = std::chrono::high_resolution_clock::now();
    const auto durDeqUs = std::chrono::duration_cast<std::chrono::microseconds>(t1d - t0d).count();

    bool same = (vecSorted.size() == deqSorted.size());
    if (same) {
        for (size_t i = 0; i < vecSorted.size(); ++i) if (vecSorted[i] != deqSorted[i]) { same = false; break; }
    }
    if (!same) {
        std::cerr << "Error: sorting results mismatch between vector and deque" << std::endl;
        return 1;
    }

    std::cout << "After:   ";
    for (size_t i = 0; i < vecSorted.size(); ++i) { if (i) std::cout << ' '; std::cout << vecSorted[i]; }
    std::cout << std::endl;

    const size_t N = inputVec.size();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Time to process a range of " << N << " elements with std::vector : " << static_cast<double>(durVecUs) << " us" << std::endl;
    std::cout << "Time to process a range of " << N << " elements with std::deque  : " << static_cast<double>(durDeqUs) << " us" << std::endl;

    return 0;
}
