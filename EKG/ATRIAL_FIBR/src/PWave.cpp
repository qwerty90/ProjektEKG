#include "PWave.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <array>

using namespace std;

namespace Ecg {
namespace AtrialFibr {

typedef QVector<double>::const_iterator Cit;

double correlation(const Cit &start1, const Cit &end1, const Cit &start2) {
  const auto end2 = start2 + distance(start1, end1);
  const auto mean1 = accumulate(start1, end1, 0.0) / distance(start1, end1);
  const auto mean2 = accumulate(start2, end2, 0.0) / distance(start2, end2);
  const auto numerator = inner_product(
      start1, end1, start2, 0.0, plus<double>(),
      [=](double a, double b) noexcept { return (a - mean1) * (b - mean2); });
  const auto squaredError1 =
      accumulate(start1, end1, 0.0, [mean1](double sum, double x) noexcept {
        return sum + (x - mean1) * (x - mean1);
      });
  const auto squaredError2 =
      accumulate(start2, end2, 0.0, [mean2](double sum, double x) noexcept {
        return sum + (x - mean2) * (x - mean2);
      });
  const auto denominator = sqrt(squaredError1) * sqrt(squaredError2);
  return numerator / denominator;
}

double correlation(const QVector<double> &v1, const QVector<double> &v2) {
  return correlation(begin(v1), end(v1), begin(v2));
}

const QVector<double> averagePWave{
  { 9.381279999999999, 9.371360000000001, 9.397920000000004, 9.425279999999997,
    9.426559999999998, 9.441279999999999, 9.481919999999997, 9.528, 9.55424,
    9.564479999999996, 9.5896, 9.62144, 9.619200000000001, 9.603359999999999,
    9.59648, 9.619520000000001, 9.607040000000001, 9.577440000000003,
    9.561119999999999, 9.559199999999997, 9.52736, 9.491519999999998,
    9.484479999999998, 9.491520000000001, 9.48032 }
};

bool biggestIteratorTooBig(const QVector<Cit> &pWaveStarts,
                           const Cit &endOfSignal) {
  return any_of(begin(pWaveStarts), end(pWaveStarts), [&](const Cit &it) {
    return distance(it + averagePWave.size(), endOfSignal) < 0;
  });
}

double pWaveOccurenceRatio(const QVector<Cit> &pWaveStarts,
                           const Cit &endOfSignal) {
  if (biggestIteratorTooBig(pWaveStarts, endOfSignal))
    throw PWaveStartTooCloseToEndOfSignal();
  const int count =
      count_if(begin(pWaveStarts), end(pWaveStarts), [](const Cit &it) {
        return 0.2 < correlation(begin(averagePWave), end(averagePWave), it);
      });
  return double(count) / pWaveStarts.size();
}
}
}
