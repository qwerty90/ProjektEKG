#pragma once
#include <vector>
#include <array>

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
  std::vector<classification> classifyIntervals(const std::vector<double> &RRIntervals);
  void countAvarageInterval(const std::vector<double> &RRIntervals);
  void countTransitions(const std::vector<classification> &classifiedIntervals);
  void normalizeMarkovTable();

  RRIntervalMethod();
};
