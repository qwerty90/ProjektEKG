#pragma once

#include <vector>
#include <algorithm>

namespace Ecg {
namespace AtrialFibr {

template <typename T> auto mean(const T &t) -> typename T::value_type {
  return std::accumulate(std::begin(t), std::end(t), typename T::value_type()) /
         t.size();
}

double correlation(const std::vector<double> &v1,
                   const std::vector<double> &v2);
}
}
