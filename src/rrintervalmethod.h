#pragma once
#include <vector>
using std::vector;

enum classification {
  Short,
  Regular,
  Long
};
class RRIntervalMethod {
  double avarageInterval;
  double markovTable[3][3];

public:
  vector<double> countRRInvervals(vector<double> RRtime);
  vector<classification> classifyIntervals(vector<double> RRIntervals);
  void countAvarageInterval(vector<double> RRIntervals);
  void countTransitions(vector<classification> classifiedIntervals);
  void normalizeMarkovTable();

  RRIntervalMethod();
};
