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
  double divergenceFactor;
  double entropyFactor;
  double pWaveOccFactor;
  void setWeights(const QString &Signal);
  double pWaveOccurenceRatioResult;
  QString signalName;
public:
  AtrialFibrApi(const QVector<double> &signal,
                const QVector<QVector<double>::const_iterator> &RPeaksIterators,
                const QVector<QVector<double>::const_iterator> &pWaveStarts,
                const QString & = QString("V5"));
  double GetRRIntEntropy() const;
  double GetRRIntDivergence() const;
  double GetPWaveAbsenceRatio() const;
  bool isAtrialFibr() const;
};
