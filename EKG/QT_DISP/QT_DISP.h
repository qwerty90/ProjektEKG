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
    /*dane wejsciowe*/	//	vector <vector <double>> signals;
	/*dane wejsciowe*/		vector <vector <int>> QRS_On, QRS_End, P_On;
	/*dane wejsciowe*/		vector <double> heartAction;
	/*dane wyjsciowe*/		vector <Evaluation> evaluations;


	/*ilosc kanalow*/			int channels;
	/*ilosc akcji serca*/		int heartBeats; 
	/*nr próbek Tpeak*/			vector <vector <int>> T_Peak;
	/*czas Tend dla wyszukiwania parabolicznego*/			vector <vector <double>> T_EndP;
	/*czas Tend dla wyszukiwania styczna*/					vector <vector <double>> T_EndT;
	/*d³ugoœci odcinków QT*/	vector <vector <double>> QT;

public:
	QT_DISP();
	void getInput(vector <vector <double>> in_signals, vector <vector <int>> in_QRS_On, vector <vector <int>> in_QRS_End, vector <vector <int>> in_P_On);
	void getInput(string path);
	void setOutput(vector <Evaluation> out_evaluation, vector <vector <double>> T_End);
	void Run();

private:
	void CalculateTend(vector<double> x, vector<double> y, int QRS_End, int P_On, int T_Peak );
	void CalculateQT();
	void Filtering(vector<double> *y, int QRS_End, int P_Onset);
	int FindTPeak(vector<double> y, int QRS_End, int P_Onset);
	double poliFitting(double* a, double* b, double* c, vector <double> x, vector <double> y, int Tpeak, int P_Onset);
	int HighestVelocity(vector <double> *x, vector <double> *y, int TPeak, int P_On);
	void Tangent(double* a, double* b, vector <double> *x, vector <double> *y,int HighestVelocityPoint);
	double CalculateTendTangent(vector <double> *x, vector <double> *y, int highestVelocity, int tPeak, int P_Onset);
	double CalculateTendParabol(vector <double> *x, vector <double> *y, int highestVelocity, int tPeak, int P_Onset);
	void DispersionEvaluation ( double gapQT, double heartAction, 
		int* BazzetState, int* FridericState, int* HodgesState, int* FraminghamState, 
		double* BazzetValue, double* FridericValue, double* HodgesValue, double* FraminghamValue);
	void EvaluateQTDisp();
	int EvaluateBazzet(double gapQT, double RR);
	int EvaluateFrideric(double gapQT, double RR);
	int EvaluateHodges(double gapQT, double heartAction);
	int EvaluateFramingham(double gapQT, double RR);


};
