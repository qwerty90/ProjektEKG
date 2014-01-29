#include "AtrialFibrApi.h"
#include <map>
#include <algorithm>

std::map<QString, std::tuple<double, double, double> > Weights{
  std::make_pair("WithPWave", std::make_tuple(0.25, 0.25, 0.5)),
  std::make_pair("WithoutPWave", std::make_tuple(0.5, 0.5, 0))
};

QVector<QString> PWaveSignalTypes = { "V1", "V2", "V5" };
void AtrialFibrApi::setWeights(const QString &Signal) {

  QString SignalType;
  if (PWaveSignalTypes.contains(Signal)) {
    SignalType = "WithPWave";
  } else {
    SignalType = "WithoutPWave";
  }
  std::tie(divergenceFactor, entropyFactor, pWaveOccFactor) =
      Weights[SignalType];
}
AtrialFibrApi::AtrialFibrApi(
    const QVector<double> &signal,
    const QVector<QVector<double>::const_iterator> &RPeaksIterators,
    const QVector<QVector<double>::const_iterator> &pWaveStarts,
    const QString &signalName)
    : pWaveStarts(pWaveStarts), endOfSignal(signal.end()), entropyResult(0.0),
      divergenceResult(0.0), pWaveOccurenceRatioResult(0.0),
      signalName(signalName) {
  rrmethod.RunRRMethod(RPeaksIterators);
  pWaveOccurenceRatioResult = pWaveOccurenceRatio(pWaveStarts, endOfSignal);
  Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                { { 0.007, 0.914, 0.013 } },
                                { { 0.019, 0.006, 0.003 } } } };
  divergenceResult = JKdivergence(rrmethod.getMarkovTable(), patternMatrix);
  entropyResult = entropy(rrmethod.getMarkovTable());
  setWeights(signalName);
}
double AtrialFibrApi::GetRRIntEntropy() const { return entropyResult; }

double AtrialFibrApi::GetRRIntDivergence() const { return divergenceResult; }

double AtrialFibrApi::GetPWaveAbsenceRatio() const {
  if (PWaveSignalTypes.contains(signalName))
    return 1 - pWaveOccurenceRatioResult;
  return std::numeric_limits<double>::quiet_NaN();
}

static const double AtrialFibrThreshold = 0.7;

bool AtrialFibrApi::isAtrialFibr() const {
  return GetRRIntDivergence() * divergenceFactor +
             GetRRIntEntropy() * entropyFactor +
             GetPWaveAbsenceRatio() * pWaveOccFactor >
         AtrialFibrThreshold;
}
