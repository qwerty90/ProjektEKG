#pragma once
//#include <vector>
#include <QVector>
#include "HRV1Bundle.h"
#include <math.h>
#include "alglib/interpolation.h"
#include "alglib/fasttransforms.h"

#define INTERVAL_LENGTH 300
#define PRECISION 0.01 //must be 0.001 < x < 0.1
#define FREQUENCY_TRESHOLD 0.4

#define HF_RANGE 0.15
#define LF_RANGE 0.04
#define VLF_RANGE 0.003


class HRV1MainModule
{
public:

	//INPUT:
    void prepare(QVector<int>* RRPeaks, int samplingFrequency = 1000); //use this function to prepare whole module (default sampling frequency - 1000 Hz)

	//OUTPUT:
    HRV1BundleStatistical evaluateStatistical(); //evaluates and returns arguments of Statistical analysys
    HRV1BundleFrequency evaluateFrequency(); //evaluates and returns frequency analysys

    HRV1MainModule(void);
    ~HRV1MainModule(void);
private:

    QVector<QVector<double>*> dividedPeaks;
    QVector<double> peaks;
    QVector<double> RRDifferences;

    double samplingFrequency;
	
    void cutPeaksVector(QVector<int>* peaks);
	void evaluateRRDifferences();

	//Statistical analysys
	HRV1BundleStatistical toReturnStatistical;

    double evaluateRRMean(QVector<double>* peaks);
    double evaluateSDNN(QVector<double>* peaks, double mean);

    double evaluateSimpleMean(QVector<double>* vector);
    double evaluateStandardDeviation(QVector<double>* vector, double mean);

	void evaluateRRMeanEntirety();
	void evaluateSDNNEntirety();
	void evaluateRMSSD();
	void evaluateNN50();
	void evaluatepNN50();
	void evaluateSDANN();
	void evaluateSDANNindex();
	void evaluateSDSD();

	
	//fourrier analysys
	alglib::spline1dinterpolant splineInterpolant;
	alglib::complex_1d_array fftArray;
	HRV1BundleFrequency toReturnFrequency;

	void evaluateSplainInterpolation();
	void evaluateFFT();

    void evaluateTP();
    void evaluateHF();
    void evaluateLF();
    void evaluateVLF();
    void evaluateULF();
    void evaluateLFHF();

    double evaluateFrequencyPower(double low, double high);
};
