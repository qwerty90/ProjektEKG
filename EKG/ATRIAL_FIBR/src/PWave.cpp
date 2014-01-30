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

  for_each(start2, end2, [](double x) { cout << x << endl; });
  cout << "==============" << endl;

  return numerator / denominator;
}

double correlation(const QVector<double> &v1, const QVector<double> &v2) {
  return correlation(begin(v1), end(v1), begin(v2));
}

const QVector<double> averagePWave{
  { -0.013162, -0.00548262, -0.00963697, -0.0177356, -0.0376679, -0.0339315,
    -0.0301946, -0.0225126, -0.0306089, -0.0189812, -0.0191872, -0.0312268,
    -0.015653, -0.0158575, -0.00028264, -0.0083756, -0.000689189, -0.000891704,
    0.00285101, 0.0144837, 0.0261169, 0.0219717, -0.00189649, 0.0176277,
    0.0332078, 0.0369542, 0.0209776, 0.0207804, 0.0245285, 0.0401114, 0.0478053,
    0.0752234, 0.0947527, 0.106393, 0.0983105, 0.0862838, 0.0860918, 0.0898452,
    0.0778203, 0.0776301, 0.0656064, 0.0614728, 0.0652292, 0.0650415, 0.0530203,
    0.0409998, 0.0368693, 0.0366842, 0.0364997, 0.0284265, 0.00851975,
    0.000447824, -0.0155129, -0.0275282, -0.0316534, -0.0436673, -0.0399017,
    -0.0321907, -0.0442025, -0.0443796 }
};

bool biggestIteratorTooBig(const QVector<Cit>::const_iterator &pWaveStartsBegin,
                           const QVector<Cit>::const_iterator &pWaveStartsEnd,
                           const Cit &endOfSignal) {
  return any_of(pWaveStartsBegin, pWaveStartsEnd, [&](const Cit &it) {
    return distance(it + averagePWave.size(), endOfSignal) < 0;
  });
}

double pWaveOccurenceRatio(const QVector<Cit>::const_iterator &pWaveStartsBegin,
                           const QVector<Cit>::const_iterator &pWaveStartsEnd,
                           const Cit &endOfSignal) {
  if (biggestIteratorTooBig(pWaveStartsBegin, pWaveStartsEnd, endOfSignal))
    throw PWaveStartTooCloseToEndOfSignal();
  const int count =
      count_if(pWaveStartsBegin, pWaveStartsEnd, [&](const Cit &it) {
        return 0.2 < correlation(begin(averagePWave), end(averagePWave), it);
      });
  return double(count) / distance(pWaveStartsBegin, pWaveStartsEnd);
}
}
}
