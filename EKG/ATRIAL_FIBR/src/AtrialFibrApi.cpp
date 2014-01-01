#include "AtrialFibrApi.h"

AtrialFibrApi::AtrialFibrApi(
    const std::vector<double> &signal,
    const std::vector<std::vector<double>::const_iterator> &RPeaksIterators,
    const std::vector<std::vector<double>::const_iterator> &pWaveStarts)
    : pWaveStarts(pWaveStarts), endOfSignal(signal.end()), entropyResult(0.0),
      divergenceResult(0.0), pWaveOccurenceRatioResult(0.0) {
  rrmethod.RunRRMethod(RPeaksIterators);
}
double AtrialFibrApi::GetEntropy() {
  if (!entropyResult)
    entropyResult = entropy(rrmethod.getMarkovTable());
  return entropyResult;
}

double AtrialFibrApi::GetDivergence() {
  if (!divergenceResult) {
    Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                  { { 0.007, 0.914, 0.013 } },
                                  { { 0.019, 0.006, 0.003 } } } };
    divergenceResult = JKdivergence(rrmethod.getMarkovTable(), patternMatrix);
  }
  return divergenceResult;
}

double AtrialFibrApi::GetPWaveOccurenceRatio() {
  if (!pWaveOccurenceRatioResult) {
    pWaveOccurenceRatioResult = pWaveOccurenceRatio(pWaveStarts, endOfSignal);
  }
  return pWaveOccurenceRatioResult;
}

bool AtrialFibrApi::isAtrialFibr() { return true; }
