#ifndef KALMAN_H
#define KALMAN_H

#ifdef _VARIADIC_MAX
#undef _VARIADIC_MAX
#endif

#define _VARIADIC_MAX 10

#include <QVector>
#include <tuple>
#include "ECG_BASELINE/3rdparty/Eigen/Core"

typedef std::tuple<QVector<double>,QVector<double>,QVector<double>> EcgParams;
typedef Eigen::MatrixXd EMatrix;
typedef Eigen::VectorXd EVector;
typedef QVector<Eigen::MatrixXd> EMatrix3;
typedef std::tuple<EMatrix, EMatrix> Linears;

namespace alglib
{
    class real_1d_array;
}

class KalmanFilter {
public:
    KalmanFilter();
    QVector<double> processKalman(const QVector<double>& ecgData);

    // Synthetic ECG model for a single point (single phase value)
    static double syntheticEcgModelPoint(const double phase, const alglib::real_1d_array &c);
    // Synthetic ECG model for a vector o phase values
    static QVector<double> syntheticEcgModelVector(const QVector<double> &phase, const QVector<double> &params);

private:
    //methods
    QVector<double> removeBaseLine(const QVector<double>&);
    QVector<double> addGaussianNoise(QVector<double>&, double SNR);

    //Kalman-smoothing related
    QVector<double> performKalmanSmoothing(const QVector<double>& modelParams, const QVector<double> &rPeaks,
                                   const QVector<double> &signal, const QVector<double>& phase, const EcgParams &signalParams);
    QVector<double> forwardFilter();
    QVector<double> backwardSmoothen();
    void copyToY(EMatrix& Y,const QVector<double>& phase, const QVector<double>& signal);
    void setEksInitParameters(const QVector<double>& modelParams, const QVector<double> &rPeaks, const QVector<double> &signal, const QVector<double>& phase, const EcgParams &signalParams);
    void constructEksMatrices(const EcgParams& signalParams);
    EMatrix3 create3dMatrix(int rows, int cols, int slices);
    EVector updateObservations(const EVector&, const EVector&);
    EVector updateStates(const EVector&);
    Linears linearizeOutputEquations();
    Linears linearizeStates(const EVector&);

    // Returns optimal synthetic model parameters for passed mean ECG beat
    QVector<double> runFitting(const QVector<double> &initParams, const QVector<double> &ecgMean, const QVector<double> &phaseMean);
    // Prepares all the data needed for fitting
    QVector<double> optimizeModelParameters(const QVector<double> selectedTs, const QVector<double> &ecgMean, const QVector<double> &ecgPhase);
    QVector<double> getEquidistPoints(const double begin, const double end, const int count);
    int findMaxIndex(const QVector<double>& absEcgMean);

    //calculate ecg parameters related
    EcgParams calculateEcgParameters(const QVector<double> &signal, QVector<double> &phase, int bins);
    void calculateFirstBin(QVector<double>& ecgMean, QVector<double>& ecgSD, QVector<double>& ecgMeanPhase, const QVector<double> &signal, const QVector<double> &phase);
    void smoothenEcgParameters(QVector<double>&ecgMean, QVector<double>&ecgSD, QVector<double>&ecgMeanPhase);
    void alignBaselineToZero(QVector<double>& ecgMean);
    void debugPrintEcgParameters(const QVector<double>&, const QVector<double>&, const QVector<double>&);
    void debugWriteEcgParametersToFile(const QVector<double>&, const QVector<double>&, const QVector<double>&);

    //calculate phase related
    QVector<double> calculatePhase(const QVector<double>& rPeaks);
    void shiftPhase(QVector<double>& phaseInput, double theta);
    void scaleToPi(QVector<double>& phaseInput);

    //find-peaks related
    QVector<double> findPeaks(const QVector<double>& input, double rrPeriod);
    bool isMaxElementInWindow(const QVector<double>& input, int i, int windowBegin, int windowEnd);
    bool isMinElementInWindow(const QVector<double>& input,int index, int indexBegin, int indexEnd);
    void establishIndexBounds(const QVector<double>& input, int i, int windowLength, int* indexBegin, int* indexEnd);
    void performPreliminaryIndexation(const QVector<double>& input, QVector<double>& rPeaks, int windowLength, bool isAbsMaxGreater);
    void filterOutFakePeaks(QVector<double>& rPeaks, int windowLength);
    void debugPrintRPeaks(const QVector<double>& rPeaks);

    //data
    //configuration
    double SNR;
    double samplingFrequency;
    double rrPeriod;
    double theta;
    double gamma;//observation covariance adaptation rate
    int bins;


    //Kalman Matrices
    EMatrix3 Pbar, Phat;
    EMatrix Y, P0, Q, R0, Pminus, Pplus, Xbar, Xhat, mem1, mem2, R;
    EVector x0, Xminus, Wmean, Vmean, Inits;
    QVector<double> ecgMean, ecgSD, ecgMeanPhase;
    int nSamples, innovationWindow, L;
    double alpha;

};

#endif // KALMAN_H
