#pragma once

#include <vector>
#include <algorithm>
#include <exception>

namespace Ecg {
namespace AtrialFibr {

template <typename T> auto mean(const T &t) -> typename T::value_type {
  return std::accumulate(std::begin(t), std::end(t), typename T::value_type()) /
         t.size();
}

double correlation(const std::vector<double> &v1,
                   const std::vector<double> &v2);

double correlation(const std::vector<double>::const_iterator &start1,
                   const std::vector<double>::const_iterator &end1,
                   const std::vector<double>::const_iterator &start2);

extern const std::vector<double> averagePWave;
double pWaveOccurenceRatio(
    const std::vector<std::vector<double>::const_iterator> &pWaveStarts,
    const std::vector<double>::const_iterator &endOfSignal);

class PWaveStartTooCloseToEndOfSignal : std::exception {};
}
}
