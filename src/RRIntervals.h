#pragma once

#include <vector>
#include <array>

namespace Ecg {
namespace AtrialFibr {

typedef std::array<std::array<double, 3>, 3>  Matrix3_3 ;
std::array<double, 3> row(const Matrix3_3 &matrix, int n);
std::array<double, 3> col(const Matrix3_3 &matrix, int n);

double entropy(const Matrix3_3 &matrix);

enum classification {
  Short,
  Regular,
  Long
};
class RRIntervalMethod {
  double avarageInterval;
  std::array<std::array<double, 3>, 3> markovTable;

public:
  std::vector<double> countRRInvervals(const std::vector<double> &RRtime);
  std::vector<classification>
  classifyIntervals(const std::vector<double> &RRIntervals);
  void countAvarageInterval(const std::vector<double> &RRIntervals);
  void countTransitions(const std::vector<classification> &classifiedIntervals);
  void normalizeMarkovTable();
  std::array<std::array<double, 3>, 3> getMarkovTable() { return markovTable; }

  RRIntervalMethod();
};
}
}
