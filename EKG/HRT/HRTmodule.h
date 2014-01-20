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
    vector<int> 			R_peaks;
	int 					frequency;
	int 					n_R;
	int						totalVEBcount;
	float					TO;
	float					TS;	
	float 					TS_B;
	vector<float> 			tacho;
	vector<float> 			VEBlist; 	//po findVEB
	vector<float> 			VEBlist2; 	//po filterVEB
	vector<vector<float>> 	RRlist;		//po createRRlist
	vector<vector<float>> 	HRT_RR;		//po filterRR
	
	static	const int				n_R_l=7;    //zakres poszukiwan dolna granica
	static	const int				n_R_u=20;   // i gorna
	static	const int				Rdist=20;	// minimalny odstep pomiedzy VEBami

	
	
	vector<float> 			findVEB();
	vector<float> 			filterVEB();
	vector<vector<float>> 	createRRlist();
	vector<vector<float>> 	filterRR();
	void 					GlobalAverage();// wylicza {TO,TS,TS_B}
	vector<float>			tachogram();

public:

	// INPUT:
	void calculateHRT(QVector<int> Rpeaks, int samplingFrequency=360);// dane wejsciowe:(wektor indeksow probek z pikami R, czestotliwosc probkowania)
	//void calculateHRT(vector<int> Rpeaks, int samplingFrequency=360);//mozna zamienic na QVector
	// OUTPUT:
	vector<float> 	get_tachogram(); //zwraca vektor reprezentujacy tachogram (25 elementow)
	int 			get_VEBcount();//zwraca liczbê znalezionych i zaakceptowanych VEB'ow 
	float 			get_TS();
	float 			get_TO();
	float			get_a();// zwraca wspolczynnik kierunkowy prostej
	float			get_b();// ax+b - do wyrysowania na tachogramie
	
};
}
#endif