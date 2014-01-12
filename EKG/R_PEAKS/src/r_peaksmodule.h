#ifndef R_PEAKSMODULE_H
#define R_PEAKSMODULE_H

#include <complex>
#include <vector>
#include <cmath>
#include <QVector>

typedef QVector<double>::iterator R_peaksIter;
typedef QVector<R_peaksIter> R_peaksIterVector;
typedef std::complex<double> ComplexDouble;
typedef QVector<ComplexDouble> VectorComplexDouble;
typedef std::pair<double, unsigned int> PairDoubleUnsignedInt;
typedef QVector<PairDoubleUnsignedInt> VectorPairDoubleUnsignedInt;


class R_peaksModule {
    private:
        static const double SAFETY_COEFFICIENT;
        static const unsigned short MAX_BPM; // Haskell and Fox: HRmax = 220 - age.
        static const double THRESHOLD_DIVISOR;
        static const unsigned int PT_LP_M;
        static const unsigned int PT_HP_M;
        static const unsigned int PT_MW_N;

        QVector<double> filteredSignal;
        R_peaksIterVector itVect;
        QVector<unsigned int> indexVector;
        R_peaksIter iter;
        double frequency;

        void fft(const QVector<double> & x, VectorComplexDouble & result);
        void ifft(const VectorComplexDouble & x, VectorComplexDouble & result);
        void fftHilbertWindowed(const QVector<double> & x, QVector<double> & result, double window_length, double window_overlap);
        void fftHilbert(const QVector<double> & x, QVector<double> & result);
        void setHilbertRPeaks(const VectorPairDoubleUnsignedInt & peaks, double dt);

        double PTLowPassFilter(double data, bool reset);
        double PTHighPassFilter(double data, bool reset);
        double PTDerivative (double data, bool reset);
        double PTSquaringFunction(double data);
        double PTMovingWindowIntegral(double data, bool reset);
        PairDoubleUnsignedInt Maximum(int begin, int end, QVector<double> & values);
        void PanTompkinsDo(QVector<double> & signal, VectorPairDoubleUnsignedInt & result);
        void PanTompkinsSetPeaks(VectorPairDoubleUnsignedInt & filter, VectorPairDoubleUnsignedInt & movingWindow);
        PairDoubleUnsignedInt Peak200MS(int & index, QVector<double> & signal);
        int QRSCheck(PairDoubleUnsignedInt rpCandidate, PairDoubleUnsignedInt rpLast, double threshold, QVector<double> & slope);
        void falka(QVector<double> & x, QVector<double> & result);
        void getEvenSamples(const QVector<double> & x, QVector<double> & result);
        void getOddSamples(const QVector<double> & x, QVector<double> & result);
        void findGreaterEqualThan(const QVector<double> & x, double coeff, QVector<unsigned int> & result);

public:
        R_peaksModule();
        R_peaksModule(const QVector<double> & filteredSignal, double freq);

        void R_peaksSetModule(const QVector<double> & filteredSignal, double freq);

        void hilbert(void);
        void panTomkins(void);
        void wavelet(void);

        const R_peaksIterVector & getPeaksIter(void);
        const QVector<unsigned int> & getPeaksIndex(void);
};

class DiffSorter {
    public:
        DiffSorter() {}
        bool operator()(const PairDoubleUnsignedInt& a, const PairDoubleUnsignedInt& b) const {
            return a.first > b.first;
        }
};

class PeaksCutSorter {
    private:
        int scope;
    public:
        PeaksCutSorter(int scope) {
            this->scope = scope;
        }
        bool operator()(const PairDoubleUnsignedInt& a, const PairDoubleUnsignedInt& b) {
            if(std::abs(a.second - b.second) <= this->scope) return false;
            else return a.second < b.second;
        }
};

QVector<double> addVector(const QVector<double> & vecA, const QVector<double> & vecB);
QVector<double> subtractVector(const QVector<double> & vecA, const QVector<double> & vecB);
QVector<double> mulVector(const QVector<double> & vecA, double B);

#endif // R_PEAKSMODULE_H
