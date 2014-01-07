#ifndef R_PEAKS_H
#define R_PEAKS_H

//#define R_PEAKS_EXPORTS
#ifdef R_PEAKS_EXPORTS
#define R_PEAKS_API __declspec(dllexport) 
#else
#define R_PEAKS_API __declspec(dllimport) 
#endif

#include<complex>
#include<vector>
#include<list>
#include<algorithm>
#include<fftw3.h>

#define HILBERT_R_PEAKS_DETECTOR 1
#define PAN_TOMPKINS_R_PEAKS_DETECTOR 2

typedef std::complex<double> ComplexDouble;
typedef std::vector<double> VectorDouble;
typedef std::vector<unsigned int> VectorUnsignedInt;
typedef std::vector<ComplexDouble> VectorComplexDouble;
typedef std::pair<double, unsigned int> PairDoubleUnsignedInt;
typedef std::vector<PairDoubleUnsignedInt> VectorPairDoubleUnsignedInt;

class RPeaks
{
	private:
		VectorDouble signal;
        VectorUnsignedInt rPeaksSamples;
		double samplingFreq;
        static const double SAFETY_COEFFICIENT;
        static const unsigned short MAX_BPM; // Haskell and Fox: HRmax = 220 - age.
        static const double THRESHOLD_DIVISOR;
        static const unsigned int PT_LP_M;
        static const unsigned int PT_HP_M;
        static const unsigned int PT_MW_N;

		//HILBERT
        VectorComplexDouble fft(VectorDouble& x);
        VectorComplexDouble ifft(VectorComplexDouble& x);
        VectorDouble fftHilbert(VectorDouble& x);
        VectorDouble fftHilbertWindowed(VectorDouble& x, double window_length, double window_overlap);
        VectorUnsignedInt getHilbertRPeaks(VectorPairDoubleUnsignedInt& peaks, double dt);
		void EngageHilbertRPeaksDetector(void);
		
		//PAN TOMPKINS		
		void EngagePanTompkinsRPeaksDetector(void);
        VectorPairDoubleUnsignedInt PanTompkinsDo(VectorDouble &signal);
		VectorUnsignedInt PanTompkinsGetPeaks(VectorPairDoubleUnsignedInt filter, VectorPairDoubleUnsignedInt movingWindow);	

        PairDoubleUnsignedInt Peak200MS(int &index, VectorDouble &signal);
        int QRSCheck(PairDoubleUnsignedInt rpCandidate, PairDoubleUnsignedInt rpLast, double threshold, VectorDouble &slope);
        PairDoubleUnsignedInt Maximum(int begin, int end, VectorDouble &values);
        double Mean(int begin, int end, VectorDouble &values);
		
		double PTLowPassFilter(double data, bool reset);
		double PTHighPassFilter(double data, bool reset);
		double PTDerivative (double data, bool reset);
		double PTSquaringFunction(double data);
		double PTMovingWindowIntegral(double data, bool reset);
		
	public:
        R_PEAKS_API RPeaks(VectorDouble& signal, double samplingFreq);
        void R_PEAKS_API EngageRPeaksDetector(unsigned char);
        VectorUnsignedInt R_PEAKS_API getRPeaksSamples(void);
};
#endif
