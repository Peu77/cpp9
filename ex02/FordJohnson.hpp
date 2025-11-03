#pragma once

#include <vector>
#include <utility>

template<typename Container>
class FordJohnson {
private:
    int comparisons_;

    static std::vector<int> generateJacobsthal(int n);

    static std::vector<int> getInsertionOrder(int n);

    void binaryInsert(Container &arr, int value, int maxPos);

public:
    FordJohnson();

    FordJohnson(const FordJohnson &other);

    FordJohnson &operator=(const FordJohnson &other);

    ~FordJohnson();

    Container sort(Container arr);

    [[nodiscard]] int getComparisons() const;
};


template<typename Container>
FordJohnson<Container>::FordJohnson() : comparisons_(0) {
}

template<typename Container>
FordJohnson<Container>::FordJohnson(const FordJohnson &other) : comparisons_(other.comparisons_) {
}

template<typename Container>
FordJohnson<Container> &FordJohnson<Container>::operator=(const FordJohnson &other) {
    if (this != &other) {
        comparisons_ = other.comparisons_;
    }
    return *this;
}

template<typename Container>
FordJohnson<Container>::~FordJohnson() = default;

template<typename Container>
std::vector<int> FordJohnson<Container>::generateJacobsthal(int n) {
    std::vector<int> j;
    j.reserve(64);
    j.push_back(0);
    j.push_back(1);
    while (j.back() < n) {
        int next = j[j.size() - 1] + 2 * j[j.size() - 2];
        j.push_back(next);
    }
    return j;
}

template<typename Container>
std::vector<int> FordJohnson<Container>::getInsertionOrder(int n) {
    std::vector<int> jac = generateJacobsthal(n);
    std::vector<int> order;
    order.reserve(n);
    std::vector<bool> used(n + 1, false);
    for (size_t i = 2; i < jac.size() && jac[i] <= n; ++i) {
        const int start = jac[i];
        const int end = jac[i - 1] + 1;
        for (int j = start; j >= end && j <= n; --j) {
            if (!used[j]) {
                order.push_back(j);
                used[j] = true;
            }
        }
    }
    for (int i = 1; i <= n; ++i) if (!used[i]) order.push_back(i);
    return order;
}

template<typename Container>
void FordJohnson<Container>::binaryInsert(Container &arr, int value, int maxPos) {
    int left = 0;
    int right = maxPos;
    while (left < right) {
        const int mid = left + (right - left) / 2;
        ++comparisons_;
        if (arr[mid] < value) left = mid + 1;
        else right = mid;
    }
    arr.insert(arr.begin() + left, value);
}

template<typename Container>
Container FordJohnson<Container>::sort(Container arr) {
    comparisons_ = 0;
    const size_t n = arr.size();
    if (n <= 1) return arr;

    std::vector<std::pair<int, int> > pairs;
    pairs.reserve(n / 2);
    int straggler = -1;
    const bool hasStraggler = (n % 2 == 1);
    if (hasStraggler) straggler = arr[n - 1];

    for (size_t i = 0; i < n - (hasStraggler ? 1 : 0); i += 2) {
        ++comparisons_;
        if (arr[i] > arr[i + 1]) pairs.emplace_back(arr[i], arr[i + 1]);
        else pairs.emplace_back(arr[i + 1], arr[i]);
    }

    Container larger;
    larger = Container();
    for (size_t i = 0; i < pairs.size(); ++i) larger.push_back(pairs[i].first);
    if (larger.size() > 1) larger = sort(larger);

    std::vector<int> smaller(pairs.size());
    for (size_t i = 0; i < pairs.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (pairs[j].first == larger[i]) {
                smaller[i] = pairs[j].second;
                break;
            }
        }
    }

    Container mainChain;
    if (!smaller.empty()) mainChain.push_back(smaller[0]);
    for (size_t i = 0; i < larger.size(); ++i) mainChain.push_back(larger[i]);

    if (smaller.size() > 1) {
        std::vector<int> order = getInsertionOrder(static_cast<int>(smaller.size()) - 1);
        for (int idx: order) {
            if (idx < static_cast<int>(smaller.size())) {
                int maxPos = idx + (idx + 1);
                if (maxPos > static_cast<int>(mainChain.size())) maxPos = static_cast<int>(mainChain.size());
                binaryInsert(mainChain, smaller[idx], maxPos);
            }
        }
    }

    if (hasStraggler) binaryInsert(mainChain, straggler, static_cast<int>(mainChain.size()));
    return mainChain;
}

template<typename Container>
int FordJohnson<Container>::getComparisons() const { return comparisons_; }
