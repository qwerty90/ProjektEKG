#ifndef HRTMODULE_H
#define HRTMODULE_H
#include <array>
#include <vector>
#include <QVector>
#include <numeric>
#include <functional>
#include <iostream>

using namespace std;

namespace HRT{

class HRTmodule
{
private:
	vector<unsigned int> 		R_peaks;
	int 				frequency;
	unsigned int			n_R;
	int				totalVEBcount;
	double				TO;
	double				TS;	
	double 				TS_B;
	//vector<double> 		tacho;
	QVector<double> 		tacho;
	vector<double> 			VEBlist; 	//po findVEB
	vector<double> 			VEBlist2; 	//po filterVEB
	vector<vector<double>> 		RRlist;		//po createRRlist
	vector<vector<double>> 		HRT_RR;		//po filterRR
	
	static	const int				n_R_l=7;    //zakres poszukiwan dolna granica
	static	const int				n_R_u=20;   // i gorna
	static	const int				Rdist=20;	// minimalny odstep pomiedzy VEBami

	
	
	vector<double> 			findVEB();
	vector<double> 			filterVEB();
	vector<vector<double>> 		createRRlist();
	vector<vector<double>> 		filterRR();
	void 				GlobalAverage();// wylicza {TO,TS,TS_B}
	vector<double>			tachogram();

public:

	// INPUT:
	void calculateHRT(QVector<unsigned int> Rpeaks, int samplingFrequency=360);// dane wejsciowe:(wektor indeksow probek z pikami R, czestotliwosc probkowania)
	//void calculateHRT(vector<unsigned int> Rpeaks, int samplingFrequency=360);//mozna zamienic na QVector
	// OUTPUT:
	//vector<double> 	get_tachogram(); //zwraca vektor reprezentujacy tachogram (25 elementow)
	QVector<double> 	get_tachogram(); //zwraca vektor reprezentujacy tachogram (25 elementow)
	int 			get_VEBcount();//zwraca liczbę znalezionych i zaakceptowanych VEB'ow 
	double 			get_TS();
	double 			get_TO();
	double			get_a();// zwraca wspolczynnik kierunkowy prostej
	double			get_b();// ax+b - do wyrysowania na tachogramie
	
};
}
#endif
