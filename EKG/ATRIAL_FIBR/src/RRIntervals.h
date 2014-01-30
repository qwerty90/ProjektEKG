#pragma once

//#include <vector>
#include <array>
#include <QVector>

namespace Ecg {
namespace AtrialFibr {

typedef std::array<std::array<double, 3>, 3> Matrix3_3;
typedef QVector<double>::const_iterator CIterators;
std::array<double, 3> row(const Matrix3_3 &matrix, int n);
std::array<double, 3> col(const Matrix3_3 &matrix, int n);

double entropy(const Matrix3_3 &matrix);
double KLdivergence(const Matrix3_3 &transitionsMatrix,
                    const Matrix3_3 &patternMatrix);
double JKdivergence(const Matrix3_3 &transitionsMatrix,
                    const Matrix3_3 &patternMatrix);
enum classification {
  Short,
  Regular,
  Long
};

QVector<classification> classifyIntervals(const QVector<int> &RRIntervals,
                                          int averageInteval);
int countAverageInterval(const QVector<int> &RRIntervals);

class RRIntervalMethod {
  Matrix3_3 markovTable;
  double countNormalization();

public:
  QVector<int> countRRInvervals(
      const QVector<CIterators>::const_iterator &RPeaksIteratorsBegin,
      const QVector<CIterators>::const_iterator &RPeaksIteratorsEnd);
  void countTransitions(const QVector<classification> &classifiedIntervals);
  void normalizeMarkovTable();
  std::array<std::array<double, 3>, 3> getMarkovTable() { return markovTable; }
  void
  RunRRMethod(const QVector<CIterators>::const_iterator &RPeaksIteratorsBegin,
              const QVector<CIterators>::const_iterator &RPeaksIteratorsEnd);
};
}
}
