#include <iostream>
#include <algorithm>
#include <functional>

#include "RRIntervals.h"
#include "PWave.h"

using namespace std;
using namespace std::placeholders;
namespace Ecg {
namespace AtrialFibr {

const static double longintervalpercentage = 1.15;
const static double shortintervalpercentage = 0.85;

QVector<double>
RRIntervalMethod::countRRInvervals(const QVector<double> &RRtime) {
  QVector<double> RRIntervals;
  transform(begin(RRtime) + 1, end(RRtime), begin(RRtime),
            back_inserter(RRIntervals), minus<double>());
  return RRIntervals;
}

QVector<classification>
RRIntervalMethod::classifyIntervals(const QVector<double> &RRIntervals) {
  QVector<classification> classifiedIntervals;
  for (const auto &interval : RRIntervals) {
    if (interval >= longintervalpercentage * averageInterval) {
      classifiedIntervals.push_back(Long);
    } else if (interval <= shortintervalpercentage * averageInterval) {
      classifiedIntervals.push_back(Short);
    } else {
      classifiedIntervals.push_back(Regular);
    }
  }
  return classifiedIntervals;
}

void RRIntervalMethod::countAverageInterval(const QVector<double> &RRIntervals) {
  averageInterval = mean(RRIntervals);
}

void RRIntervalMethod::countTransitions(
    const QVector<classification> &classifiedIntervals) {

  markovTable.fill({ { 0.0, 0.0, 0.0 } });
  for (auto it = classifiedIntervals.begin();
       it != classifiedIntervals.end() - 1; ++it) {
    markovTable[*it][*(it + 1)] += 1;
  }
}

double RRIntervalMethod::countNormalization() {
  double sum = 0;
  for (int i = 0; i < 3; ++i) {
    auto _row = row(markovTable, i);
    sum += accumulate(begin(_row), end(_row), 0.0);
  }
  return sum;
}

void RRIntervalMethod::normalizeMarkovTable() {
  double normalizationValue = countNormalization();
  for (int i = 0; i < 3; ++i) {
    array<double, 3> ans;
    auto _row = row(markovTable, i);
    transform(begin(_row), end(_row), begin(ans),
              bind(divides<double>(), _1, normalizationValue));
    markovTable[i] = ans;
  }
}
void RRIntervalMethod::RunRRMethod(const QVector<CIterators> &RPeaksIterators) {
  QVector<double> RPeaks;
  for (QVector<CIterators>::const_iterator iter = RPeaksIterators.begin();
       iter < RPeaksIterators.end(); ++iter) {
    RPeaks.push_back(**iter);
  }
  QVector<double> RRIntervals = countRRInvervals(RPeaks);
  countAverageInterval(RRIntervals);
  countTransitions(classifyIntervals(RRIntervals));
  normalizeMarkovTable();
}

std::array<double, 3> row(const Matrix3_3 &matrix, int n) { return matrix[n]; }

std::array<double, 3> col(const Matrix3_3 &matrix, int n) {
  array<double, 3> ans;
  transform(begin(matrix), end(matrix), begin(ans),
            [n](const array<double, 3> &row) { return row[n]; });
  return ans;
}

double entropy(const Matrix3_3 &matrix) {
  array<double, 3> H;
  for (int i = 0; i < 3; i++) {
    const auto _row = row(matrix, i);
    array<double, 3> multipliedByLog;
    transform(begin(_row), end(_row), begin(multipliedByLog),
              [](double x) { return x * log(x) / log(2); });
    H[i] = accumulate(begin(multipliedByLog), end(multipliedByLog), 0.0);
  }

  array<double, 3> P;
  for (int i = 0; i < 3; i++) {
    const auto column = col(matrix, i);
    P[i] = accumulate(begin(column), end(column), 0.0);
  }

  return -inner_product(begin(P), end(P), begin(H), 0.0);
}

double KLdivergence(const Matrix3_3 &transitionsMatrix,
                    const Matrix3_3 &patternMatrix) {
  double sum = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      sum += log(patternMatrix[i][j] / transitionsMatrix[i][j]) *
             patternMatrix[i][j];
    }
  }
  return sum;
}
double JKdivergence(const Matrix3_3 &transitionsMatrix,
                    const Matrix3_3 &patternMatrix) {
  Matrix3_3 M;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; ++j) {
      M[i][j] = 0.5 * (patternMatrix[i][j] + transitionsMatrix[i][j]);
    }
  }
  return 0.5 *
         (KLdivergence(M, patternMatrix) + KLdivergence(M, transitionsMatrix));
}
}
}
