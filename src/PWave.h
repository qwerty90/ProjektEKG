#pragma once

#include <vector>
#include <algorithm>

namespace Ecg {
namespace AtrialFibr {

template <typename T>
auto mean(T &&t) -> typename std::remove_reference<T>::type::value_type {
  return std::accumulate(std::begin(t), std::end(t), 0.0) / t.size();
}

double correlation(const std::vector<double> &v1,
                   const std::vector<double> &v2);
}
}
