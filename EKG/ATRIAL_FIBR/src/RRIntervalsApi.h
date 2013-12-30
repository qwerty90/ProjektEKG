#pragma once

#include <vector>
#include <array>
#include "RRIntervals.h"
using namespace Ecg::AtrialFibr;

class RRIntervalsApi {
  RRIntervalMethod rrmethod;
  double entropyResult;
  double divergenceResult;

public:
  RRIntervalsApi(std::vector<CIterators> &RPeaksIterators);
  double GetEntropy();
  double GetDivergence();
};
