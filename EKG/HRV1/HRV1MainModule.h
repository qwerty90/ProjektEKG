#pragma once
#include <vector>
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
	static void prepare(std::vector<int>* RRPeaks, int samplingFrequency = 1000); //use this function to prepare whole module (default sampling frequency - 1000 Hz)

	//OUTPUT:
	static HRV1BundleStatistical evaluateStatistical(); //evaluates and returns arguments of Statistical analysys
	static HRV1BundleFrequency evaluateFrequency(); //evaluates and returns frequency analysys
private:
	static HRV1MainModule* instance;
	static void createInstance();

	std::vector<std::vector<int>*> dividedPeaks;
	std::vector<int> peaks;
	std::vector<double> RRDifferences;

	int samplingFrequency;
	
	~HRV1MainModule(void);

	void cutPeaksVector(std::vector<int>* peaks);
	void evaluateRRDifferences();

	//Statistical analysys
	HRV1BundleStatistical toReturnStatistical;

	double evaluateRRMean(std::vector<int>* peaks);
	double evaluateSDNN(std::vector<int>* peaks, double mean);

	double evaluateSimpleMean(std::vector<double>* vector);
	double evaluateSimpleMean(std::vector<int>* vector);
	double evaluateStandardDeviation(std::vector<double>* vector, double mean);
	double evaluateStandardDeviation(std::vector<int>* vector, double mean);

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
