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
class RRIntervalMethod {
  double averageInterval;
  Matrix3_3 markovTable;
  double countNormalization();

public:
  QVector<double> countRRInvervals(const QVector<double> &RRtime);
  QVector<classification>
  classifyIntervals(const QVector<double> &RRIntervals);
  void countAverageInterval(const QVector<double> &RRIntervals);
  void countTransitions(const QVector<classification> &classifiedIntervals);
  void normalizeMarkovTable();
  std::array<std::array<double, 3>, 3> getMarkovTable() { return markovTable; }
  void RunRRMethod(const QVector<CIterators> &RPeaksIterators);
};
}
}
