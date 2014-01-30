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
                const QString & = QString("V1"));
  double GetRRIntEntropy() const;
  double GetRRIntDivergence() const;
  double GetPWaveAbsenceRatio() const;
  bool isAtrialFibr() const;
};

typedef QVector<double>::const_iterator Cit;

QVector<Cit>::const_iterator closestPWave(QVector<Cit>::const_iterator pBegin,
                                          QVector<Cit>::const_iterator pEnd,
                                          Cit rpeak);

QVector<QVector<Cit>::const_iterator>
calcRWaveSets(const QVector<Cit>::const_iterator &rpeaksBegin,
              const QVector<Cit>::const_iterator &rpeaksEnd, int step);

using namespace std;

typedef tuple<QVector<Cit>::const_iterator, QVector<Cit>::const_iterator>
calcPair;
QVector<calcPair> calcSets(QVector<Cit>::const_iterator pBegin,
                           QVector<Cit>::const_iterator pEnd,
                           QVector<Cit>::const_iterator rBegin,
                           QVector<Cit>::const_iterator rEnd);
