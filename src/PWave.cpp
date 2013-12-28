#include "PWave.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <array>

using namespace std;

namespace Ecg {
namespace AtrialFibr {

double correlation(const vector<double> &v1, const vector<double> &v2) {
  const auto mean1 = mean(v1);
  const auto mean2 = mean(v2);
  const auto numerator = inner_product(
      begin(v1), end(v1), begin(v2), 0.0, plus<double>(),
      [=](double a, double b) noexcept { return (a - mean1) * (b - mean2); });
  const auto squaredError1 = accumulate(begin(v1), end(v1), 0.0,
                                        [mean1](double sum, double x) noexcept {
    return sum + (x - mean1) * (x - mean1);
  });
  const auto squaredError2 = accumulate(begin(v2), end(v2), 0.0,
                                        [mean2](double sum, double x) noexcept {
    return sum + (x - mean2) * (x - mean2);
  });
  const auto denominator = sqrt(squaredError1) * sqrt(squaredError2);
  return numerator / denominator;
}

const vector<double> averagePWave{
  { 9.381279999999999, 9.371360000000001, 9.397920000000004, 9.425279999999997,
    9.426559999999998, 9.441279999999999, 9.481919999999997, 9.528, 9.55424,
    9.564479999999996, 9.5896, 9.62144, 9.619200000000001, 9.603359999999999,
    9.59648, 9.619520000000001, 9.607040000000001, 9.577440000000003,
    9.561119999999999, 9.559199999999997, 9.52736, 9.491519999999998,
    9.484479999999998, 9.491520000000001, 9.48032 }
};

double pWaveOccurenceRatio(
    const std::vector<std::vector<double>::const_iterator> &pWaveStarts) {

  const int count = count_if(begin(pWaveStarts), end(pWaveStarts),
                             [](std::vector<double>::const_iterator it) {
    return 0.2 < correlation(averagePWave,
                             vector<double>(it, it + averagePWave.size()));
  });
  return double(count) / pWaveStarts.size();
}
}
}
