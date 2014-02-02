#include "AtrialFibrApi.h"
#include <map>
#include <algorithm>
using namespace std;
map<QString, tuple<double, double, double> > Weights{
  make_pair("WithPWave", make_tuple(0.25, 0.25, 0.5)),
  make_pair("WithoutPWave", make_tuple(0.5, 0.5, 0))
};

QVector<QString> PWaveSignalTypes = { "V1" };
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

const Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                    { { 0.007, 0.914, 0.013 } },
                                    { { 0.019, 0.006, 0.003 } } } };

AtrialFibrApi::AtrialFibrApi(
    const QVector<double> &signal,
    const QVector<QVector<double>::const_iterator> &RPeaksIterators,
    const QVector<QVector<double>::const_iterator> &pWaveStarts,
    const QString &signalName)
    : pWaveStarts(pWaveStarts), endOfSignal(signal.end()), entropyResult(0.0),
      divergenceResult(0.0), pWaveOccurenceRatioResult(0.0),
      signalName(signalName) {
  int window = 100;
  setWeights(signalName);
  const auto sets = calcSets(begin(pWaveStarts), end(pWaveStarts),
                             begin(RPeaksIterators) + 100,
                             end(RPeaksIterators) - 120, window);
  double maxPWaveOccurenceRatioResult = 0.0;
  double maxDivergenceResult = 0.0;
  double maxEntropyResult = 0.0;
  double maxSum = 0.0;
  for (const auto &set : sets) {
    rrmethod.RunRRMethod(get<0>(set), get<0>(set) + window);
    const double pWaveOccurenceRatioResult =
        pWaveOccurenceRatio(get<1>(set), get<1>(set) + window, endOfSignal);
    const double divergenceResult =
        JKdivergence(rrmethod.getMarkovTable(), patternMatrix);
    const double entropyResult = entropy(rrmethod.getMarkovTable());
    const double sum =
        divergenceFactor * divergenceResult + entropyResult * entropyFactor +
        (1 - pWaveOccurenceRatioResult) * pWaveOccFactor;
    if (sum > maxSum) {
      maxPWaveOccurenceRatioResult = pWaveOccurenceRatioResult;
      maxDivergenceResult = divergenceResult;
      maxEntropyResult = entropyResult;
      maxSum = sum;
    }
    this->divergenceResult = maxDivergenceResult;
    this->entropyResult = maxEntropyResult;
    this->pWaveOccurenceRatioResult = maxPWaveOccurenceRatioResult;
  }
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

typedef QVector<double>::const_iterator Cit;

QVector<Cit>::const_iterator closestPWave(QVector<Cit>::const_iterator pBegin,
                                          QVector<Cit>::const_iterator pEnd,
                                          Cit rpeak) {
  const auto ans = find_if(pBegin, pEnd, [ = ](Cit cit) {
    return distance(cit, rpeak) < 0;
  });
  return ans - 1;
}

QVector<QVector<Cit>::const_iterator>
calcRWaveSets(const QVector<Cit>::const_iterator &rpeaksBegin,
              const QVector<Cit>::const_iterator &rpeaksEnd, int step) {
  const auto dist = distance(rpeaksBegin, rpeaksEnd);
  QVector<QVector<Cit>::const_iterator> answer;
  answer.reserve(dist / step - 1);
  for (auto it = rpeaksBegin; distance(it, rpeaksEnd) > step; it += step) {
    answer.push_back(it);
  }
  return answer;
}

using namespace std;

typedef tuple<QVector<Cit>::const_iterator, QVector<Cit>::const_iterator>
    calcPair;
QVector<calcPair> calcSets(QVector<Cit>::const_iterator pBegin,
                           QVector<Cit>::const_iterator pEnd,
                           QVector<Cit>::const_iterator rBegin,
                           QVector<Cit>::const_iterator rEnd, int window) {
  const auto rWaveSets = calcRWaveSets(rBegin, rEnd, window);
  QVector<calcPair> answer;
  transform(begin(rWaveSets), end(rWaveSets), back_inserter(answer),
            [ = ](QVector<Cit>::const_iterator rpeak) {
    return make_tuple(rpeak, closestPWave(pBegin, pEnd, *rpeak));
  });
  return answer;
}
