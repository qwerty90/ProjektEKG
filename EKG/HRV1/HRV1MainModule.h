#pragma once
//#include <vector>
#include <QVector>
#include "HRV1Bundle.h"
#include <math.h>
#include "alglib/interpolation.h"
#include "alglib/fasttransforms.h"

#define INTERVAL_LENGTH 300
#define PRECISION 0.05 //must be 0.001 < x < 1


class HRV1MainModule
{
public:

	//INPUT:
    static void prepare(QVector<int>* RRPeaks, int samplingFrequency = 1000); //use this function to prepare whole module (default sampling frequency - 1000 Hz)

	//OUTPUT:
	static HRV1BundleStatistical evaluateStatistical(); //evaluates and returns arguments of Statistical analysys
	static HRV1BundleFrequency evaluateFrequency(); //evaluates and returns frequency analysys

    //~HRV1MainModule(void);
private:
	static HRV1MainModule* instance;
	static void createInstance();

    QVector<QVector<int>*> dividedPeaks;
    QVector<int> peaks;
    QVector<double> RRDifferences;

	int samplingFrequency;
	


    void cutPeaksVector(QVector<int>* peaks);
	void evaluateRRDifferences();

	//Statistical analysys
	HRV1BundleStatistical toReturnStatistical;

    double evaluateRRMean(QVector<int>* peaks);
    double evaluateSDNN(QVector<int>* peaks, double mean);

    double evaluateSimpleMean(QVector<double>* vector);
    double evaluateSimpleMean(QVector<int>* vector);
    double evaluateStandardDeviation(QVector<double>* vector, double mean);
    double evaluateStandardDeviation(QVector<int>* vector, double mean);

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
};
