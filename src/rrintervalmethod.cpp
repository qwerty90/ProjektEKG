#include "rrintervalmethod.h"
#include <iostream>
#include <algorithm>
#include <functional>
using namespace std;
const static double longintervalpercentage = 1.15;
const static double shortintervalpercentage = 0.85;

RRIntervalMethod::RRIntervalMethod() {}

vector<double>
RRIntervalMethod::countRRInvervals(const vector<double> &RRtime) {
  vector<double> RRIntervals;
  for (auto it = RRtime.begin(); it != RRtime.end() - 1; ++it) {
    RRIntervals.push_back(*(it + 1) - *it);
  }
  return RRIntervals;
}

vector<classification>
RRIntervalMethod::classifyIntervals(const vector<double> &RRIntervals) {
  vector<classification> classifiedIntervals;
  for (const auto &interval : RRIntervals) {
    if (interval >= longintervalpercentage * avarageInterval) {
      classifiedIntervals.push_back(Long);
    } else if (interval <= shortintervalpercentage * avarageInterval) {
      classifiedIntervals.push_back(Short);
    } else {
      classifiedIntervals.push_back(Regular);
    }
  }
  return classifiedIntervals;
}

void RRIntervalMethod::countAvarageInterval(const vector<double> &RRIntervals) {
  avarageInterval =
      accumulate(begin(RRIntervals), end(RRIntervals), 0.0, plus<double>()) /
      RRIntervals.size();
}

void RRIntervalMethod::countTransitions(
    const vector<classification> &classifiedIntervals) {
  std::fill(&markovTable[0][0], &markovTable[3][0], 0.0);
  for (auto it = classifiedIntervals.begin();
       it != classifiedIntervals.end() - 1; ++it) {
    markovTable[*it][*(it + 1)] += 1;
  }
}
void RRIntervalMethod::normalizeMarkovTable() {}
