#include "AtrialFibrApi.h"

AtrialFibrApi::AtrialFibrApi(
        const QVector<double> &signal,
        const QVector<QVector<double>::const_iterator> &RPeaksIterators,
        const QVector<QVector<double>::const_iterator> &pWaveStarts)
    : pWaveStarts(pWaveStarts), endOfSignal(signal.end()), entropyResult(0.0),
      divergenceResult(0.0), pWaveOccurenceRatioResult(0.0) {
  rrmethod.RunRRMethod(RPeaksIterators);
}
double AtrialFibrApi::GetRRIntEntropy() {
  if (!entropyResult)
    entropyResult = entropy(rrmethod.getMarkovTable());
  return entropyResult;
}

double AtrialFibrApi::GetRRIntDivergence() {
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

static const double divergenceFactor = 1;
static const double entropyFactor = 1;
static const double pWaveOccFactor = 1;
static const double AtrialFibrThreshold = 2;

bool AtrialFibrApi::isAtrialFibr() {
  if (GetRRIntDivergence() * divergenceFactor + GetRRIntEntropy() * entropyFactor +
          GetPWaveOccurenceRatio() * pWaveOccFactor >
      AtrialFibrThreshold)
    return true;
  return false;
}
