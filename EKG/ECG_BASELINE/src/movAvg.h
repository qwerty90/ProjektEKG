#pragma once

#include <QVector>

class QPointF;

QVector<double> processMovAvg(const QVector<double> &signal, int samplingFrequencyHz, double averagingTimeS);

QVector<double> processMovAvg(const QVector<double>& signal, int windowSize);

QVector<QPointF> movAvgMagPlot(int samplingFrequencyHz, double averagingTimeS);


