#include "PWave.h"

#include <algorithm>
#include <numeric>

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
}
}
