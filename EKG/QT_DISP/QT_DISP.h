#include <vector>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "Evaluation.h"

#define M_PI       3.14159265358979323846

using namespace std;

class QT_DISP
{
private:
	/*dane wejsciowe*/		vector <double> signals2; 
	/*dane wejsciowe*/		vector <int> QRS_On, QRS_End, P_On;
	/*dane wejsciowe*/		double heartAction;
	/*dane wejsciowe*/		double samplingFrequency;
	/*dane wyjsciowe*/		vector <Evaluation> evaluations;
	


//	/*ilosc kanalow*/			int channels;
	/*ilosc akcji serca*/				int heartBeats; 
	/*nr próbek Tpeak*/					vector <int> T_Peak;
	/*czas Tend dla wyszukiwania parabolicznego*/			vector <double> T_EndP;
	/*czas Tend dla wyszukiwania styczna*/					vector <double> T_EndT;
	/*d³ugoœci odcinków QT dla wyszukiwania parabolicznego*/		vector <double> QTP;
	/*d³ugoœci odcinków QT dla wyszukiwania styczna*/				vector <double> QTT;

public:
	QT_DISP();
	void getInput(vector <double> in_signals2, vector <int> in_QRS_On, vector <int> in_QRS_End, vector <int> in_P_On, double in_samplingFrequency);
	void getInput(string path);
	void setOutput(vector <Evaluation> out_evaluation, vector <double> T_End);
	void Run();

private:
	void CalculateTend(vector<double> x, vector<double> y, int QRS_End, int P_On, int T_Peak );
	void CalculateQT(double QRS_OnTime, int number_T_End_QT);
	void Filtering(vector<double> *y, int QRS_End, int P_Onset);
	int FindTPeak(vector<double> *y, int QRS_End, int P_Onset);
	double poliFitting(double* a, double* b, double* c, vector <double> x, vector <double> y, int Tpeak, int P_Onset);
	int HighestVelocity(vector <double> *x, vector <double> *y, int TPeak, int P_On);
	void Tangent(double* a, double* b, vector <double> *x, vector <double> *y,int HighestVelocityPoint);
	double CalculateTendTangent(vector <double> *x, vector <double> *y, int highestVelocity, int tPeak, int P_Onset);
	double CalculateTendParabol(vector <double> *x, vector <double> *y, int highestVelocity, int tPeak, int P_Onset);
	void DispersionEvaluation ( double gapQT, double heartAction, 
		int* BazzetState, int* FridericState, int* HodgesState, int* FraminghamState, 
		double* BazzetValue, double* FridericValue, double* HodgesValue, double* FraminghamValue);
	void EvaluateQTDisp(double QTT, double QTP);
	int EvaluateBazzet(double gapQT, double RR);
	int EvaluateFrideric(double gapQT, double RR);
	int EvaluateHodges(double gapQT, double heartAction);
	int EvaluateFramingham(double gapQT, double RR);

	vector <double> returnTEnd();
	Evaluation returnEvaluations(int number);

};
