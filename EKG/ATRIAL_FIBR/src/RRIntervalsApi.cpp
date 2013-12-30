#include "RRIntervalsApi.h"
RRIntervalsApi::RRIntervalsApi(std::vector<CIterators> &RPeaksIterators)
    : entropyResult(0.0), divergenceResult(0.0) {
  rrmethod.RunRRMethod(RPeaksIterators);
}

double RRIntervalsApi::GetEntropy() {
  if (!entropyResult)
    entropyResult = entropy(rrmethod.getMarkovTable());
  return entropyResult;
}

double RRIntervalsApi::GetDivergence() {
  if (!divergenceResult) {
    Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                  { { 0.007, 0.914, 0.013 } },
                                  { { 0.019, 0.006, 0.003 } } } };
    divergenceResult = JKdivergence(rrmethod.getMarkovTable(), patternMatrix);
  }
  return divergenceResult;
}
