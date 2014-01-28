#pragma once
#include "RRIntervals.h"
#include "PWave.h"
#include <QVector>
#include <QString>
using namespace Ecg::AtrialFibr;
class AtrialFibrApi {
  RRIntervalMethod rrmethod;
  const QVector<QVector<double>::const_iterator> pWaveStarts;
  const QVector<double>::const_iterator endOfSignal;
  double entropyResult;
  double divergenceResult;
  double pWaveOccurenceRatioResult;

public:
  AtrialFibrApi(const QVector<double> &signal,
                const QVector<QVector<double>::const_iterator> &RPeaksIterators,
                const QVector<QVector<double>::const_iterator> &pWaveStarts,
                const QString & = QString(""));
  double GetRRIntEntropy() const;
  double GetRRIntDivergence() const;
  double GetPWaveAbsenceRatio() const;
  bool isAtrialFibr() const;
};
