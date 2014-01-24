#include "AtrialFibrApi.h"

AtrialFibrApi::AtrialFibrApi(
    const QVector<double> &signal,
    const QVector<QVector<double>::const_iterator> &RPeaksIterators,
    const QVector<QVector<double>::const_iterator> &pWaveStarts)
    : pWaveStarts(pWaveStarts), endOfSignal(signal.end()), entropyResult(0.0),
      divergenceResult(0.0), pWaveOccurenceRatioResult(0.0) {
  rrmethod.RunRRMethod(RPeaksIterators);
  pWaveOccurenceRatioResult = pWaveOccurenceRatio(pWaveStarts, endOfSignal);
  Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                { { 0.007, 0.914, 0.013 } },
                                { { 0.019, 0.006, 0.003 } } } };
  divergenceResult = JKdivergence(rrmethod.getMarkovTable(), patternMatrix);
  entropyResult = entropy(rrmethod.getMarkovTable());
}
double AtrialFibrApi::GetRRIntEntropy() const { return entropyResult; }

double AtrialFibrApi::GetRRIntDivergence() const { return divergenceResult; }

double AtrialFibrApi::GetPWaveAbsenceRatio() const {
  return 1 - pWaveOccurenceRatioResult;
}

static const double divergenceFactor = 0.25;
static const double entropyFactor = 0.25;
static const double pWaveOccFactor = 0.5;
static const double AtrialFibrThreshold = 0.7;

bool AtrialFibrApi::isAtrialFibr() const {
  return GetRRIntDivergence() * divergenceFactor +
             GetRRIntEntropy() * entropyFactor +
             GetPWaveAbsenceRatio() * pWaveOccFactor >
         AtrialFibrThreshold;
}
