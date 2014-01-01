#pragma once
#include "RRIntervals.h"
#include "PWave.h"
using namespace Ecg::AtrialFibr;
class AtrialFibrApi {
  RRIntervalMethod rrmethod;
  const std::vector<CIterators> pWaveStarts;
  const CIterators endOfSignal;
  double entropyResult;
  double divergenceResult;
  double pWaveOccurenceRatioResult;

public:
  AtrialFibrApi(const std::vector<double> &signal,
                const std::vector<CIterators> &RPeaksIterators,
                const std::vector<CIterators> &pWaveStarts);
  double GetEntropy();
  double GetDivergence();
  double GetPWaveOccurenceRatio();
  bool isAtrialFibr();
};
