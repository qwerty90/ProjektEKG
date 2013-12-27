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
  vector<double> countRRInvervals(const vector<double>& RRtime);
  vector<classification> classifyIntervals(const vector<double>& RRIntervals);
  void countAvarageInterval(const vector<double> &RRIntervals);
  void countTransitions(const vector<classification>& classifiedIntervals);
  void normalizeMarkovTable();

  RRIntervalMethod();
};
