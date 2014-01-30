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
  { -0.0358029, -0.0241104, -0.0281965, -0.0322824, -0.036368, -0.060177,
    -0.0327043, -0.0367891, -0.0369289, -0.0370685, -0.0372078, -0.0373468,
    -0.0295962, -0.0297347, -0.0338176, -0.0339556, -0.0301486, -0.0184518,
    -0.00675482, -0.0068917, -0.0149178, -0.0189988, -0.0191349, -0.0113812,
    -0.00362731, 0.0238505, 0.0394944, 0.0433044, 0.04317, 0.0745936, 0.0981281,
    0.101939, 0.0939165, 0.0661704, 0.0463141, 0.0422369, 0.0421047, 0.0498623,
    0.045786, 0.0417099, 0.0258, 0.00989043, 0.0137047, 0.0135745, 0.0213341,
    0.00937035, -0.0025931, -0.00666681, -0.0265191, -0.0266475, -0.0464992,
    -0.0426823, -0.0467546, -0.0389924, -0.0312298, -0.0313565, -0.0393722,
    -0.043443, -0.0711817, -0.0673624 }
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
