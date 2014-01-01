#pragma once
#include "RRIntervals.h"
#include "PWave.h"
using namespace Ecg::AtrialFibr;
class AtrialFibrApi {
  RRIntervalMethod rrmethod;
  const std::vector<std::vector<double>::const_iterator> pWaveStarts;
  const std::vector<double>::const_iterator endOfSignal;
  double entropyResult;
  double divergenceResult;
  double pWaveOccurenceRatioResult;

public:
  AtrialFibrApi(const std::vector<double> &signal,
                const std::vector<std::vector<double>::const_iterator> &RPeaksIterators,
                const std::vector<std::vector<double>::const_iterator> &pWaveStarts);
  double GetRRIntEntropy();
  double GetRRIntDivergence();
  double GetPWaveOccurenceRatio();
  bool isAtrialFibr();
};
