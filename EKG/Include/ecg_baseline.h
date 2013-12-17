#pragma once

#include <QtCore/qglobal.h>

#if defined(ECG_BASELINE_LIBRARY)
#  define ECG_BASELINESHARED_EXPORT Q_DECL_EXPORT
#else
#  define ECG_BASELINESHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QVector>
#include <complex>

typedef std::complex<double> complex;

// Second-order (biquadratic) IIR filtering
ECG_BASELINESHARED_EXPORT QVector<double> sosfilt(const QVector<double>& sosMatrix,
                                                  double gain,
                                                  const QVector<double>& x);
// Second-order (biquadratic) IIR Zero-phase digital filtering
ECG_BASELINESHARED_EXPORT QVector<double> sosfiltfilt(const QVector<double>& sosMatrix,
                                                      float gain,
                                                      const QVector<complex>& poles,
                                                      const QVector<double>& x);
ECG_BASELINESHARED_EXPORT QVector<double> processButter(const QVector<double>& signal);
ECG_BASELINESHARED_EXPORT QVector<double> processMovAvg(const QVector<double>& signal, int windowSize);
// ECG_BASELINESHARED_EXPORT QVector<double> processModMovAvg(const QVector<double>& signal, int windowSize, int samplingRate);
ECG_BASELINESHARED_EXPORT QVector<double> processSGolay(const QVector<double>& ecgData,
                                                        const int deg = 3,
                                                        const int frame = 500,
                                                        QVector<double>* baselineModel = 0);
