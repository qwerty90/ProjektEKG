#pragma once

//#include <vector>
#include <algorithm>
#include <exception>
#include <QVector>

namespace Ecg {
namespace AtrialFibr {

template <typename T> auto mean(const T &t) -> typename T::value_type {
  return std::accumulate(std::begin(t), std::end(t), typename T::value_type()) /
         t.size();
}

double correlation(const QVector<double> &v1,
                   const QVector<double> &v2);

double correlation(const QVector<double>::const_iterator &start1,
                   const QVector<double>::const_iterator &end1,
                   const QVector<double>::const_iterator &start2);

extern const QVector<double> averagePWave;
double pWaveOccurenceRatio(
    const QVector<QVector<double>::const_iterator> &pWaveStarts,
    const QVector<double>::const_iterator &endOfSignal);

class PWaveStartTooCloseToEndOfSignal : std::exception {};
}
}
