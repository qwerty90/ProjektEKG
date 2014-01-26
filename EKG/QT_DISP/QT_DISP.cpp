// medyczne.cpp : Defines the entry point for the console application.
#include "QT_DISP.h"
using namespace std;

QT_DISP::QT_DISP()
{
	evaluations.insert(evaluations.begin(), Evaluation("Parabol - Framigham"));
	evaluations.insert(evaluations.begin(), Evaluation("Parabol - Hodges"));
	evaluations.insert(evaluations.begin(), Evaluation("Parabol - Frideric"));
	evaluations.insert(evaluations.begin(), Evaluation("Parabol - Bazzet"));
	evaluations.insert(evaluations.begin(), Evaluation("Tangent - Framigham"));
	evaluations.insert(evaluations.begin(), Evaluation("Tangent - Hodges"));
	evaluations.insert(evaluations.begin(), Evaluation("Tangent - Frideric"));
	evaluations.insert(evaluations.begin(), Evaluation("Tangent - Bazzet"));
}

void QT_DISP::getInput (vector<double> in_signals2, vector <int> in_QRS_On, vector <int> in_QRS_End, vector <int> in_P_On, double in_samplingFrequency)
{
	signals2 = in_signals2;
	QRS_On = in_QRS_On;
	QRS_End = in_QRS_End;
	P_On = in_P_On;
	samplingFrequency = in_samplingFrequency;

	/*heartBeats = QRS_On.size() - 1;
	channels = signals2.size() - 1;
	T_Peak.resize(channels);
	T_EndP.resize(channels);
	T_EndT.resize(channels);
	for(int i = 0; i < channels; ++i)
	{
		T_Peak[i].resize(heartBeats);
		T_EndT[i].resize(heartBeats);
		T_EndP[i].resize(heartBeats);
	}*/
}

void QT_DISP::getInput(string path)
{
	fstream file;
	file.open(path, ios::in);

	if(file.good() == true)
	{
		int count;
		file >> count;
		file >> samplingFrequency;
		
		signals2 = vector <double>(count, 0.0);
		
		for(int i = 0; i < count; ++i)
		{
			file >> signals2[i];
		}
	
		file >> heartBeats;

		QRS_On = vector <int>(heartBeats + 1, 0);
		QRS_End = vector <int>(heartBeats + 1, 0);
		P_On = vector <int>(heartBeats, 0);

		for(int i = 0; i < heartBeats + 1; ++i)
		{
			file >> QRS_On[i];
		}
		for(int i = 0; i < heartBeats + 1; ++i)
		{
			file >> QRS_End[i];
		}
		for(int i = 0; i < heartBeats; ++i)
		{
			file >> P_On[i];
		}

		T_Peak.resize(heartBeats);
		T_EndT.resize(heartBeats);
		T_EndP.resize(heartBeats);

		file.close();
	}
}
	
// Funkcja wyjsciowa - konce zalamka T
vector <double> QT_DISP::returnTEnd()
{
	return T_EndP;
}


//zwraca dana ewaluacje
//0 - Bazzet Tangent
//1 - Frideric Tangent
//2 - Hodges Tangent
//3 - Framingham Tangent
//4 - Bazzet Parabol
//5 - Frideric Parabol
//6 - Hodges Parabol
//7 - Framingham Parabol
Evaluation QT_DISP::returnEvaluations(int number)
{
	return evaluations[number];
}

void QT_DISP::setOutput(vector <Evaluation> out_evaluations, vector <double> T_End)
{

	//out_evaluations = evaluations;
	out_evaluations = evaluations;
	T_End = T_EndP;
	
}

void QT_DISP::Run()
{
        for(int j = 0; j < heartBeats; ++j)
        {
            //vector <double> x (signals2.begin() + QRS_On[j], signals2.begin() + QRS_On[j + 1]);
            vector <double> y (signals2.begin() + QRS_On[j], signals2.begin() + QRS_On[j + 1]);
			vector <double> x;

			x.resize(QRS_On[j+1]-QRS_On[j]);
			int iterator = 0;
			for (int i=QRS_On[j]; i<QRS_On[j+1];i++)
			{
				x[iterator] = i/samplingFrequency;
				iterator++;
			}

            int iQRS_On = QRS_On[j] - QRS_On[j];
            int iQRS_End = QRS_End[j] - QRS_On[j];
            int iP_On = P_On[j] - QRS_On[j];

			//filtrowanie
			Filtering(&y, iQRS_End, iP_On);
			Filtering(&y, iQRS_End, iP_On);
			Filtering(&y, iQRS_End, iP_On);
			Filtering(&y, iQRS_End, iP_On);

			//odnalezienie potrzebnych do wyszukania konca zalamka miejsc dodatkowych
			int iT_Peak = FindTPeak(y,iQRS_End,iP_On); 
			T_Peak[j] = iT_Peak;

			int highestvelocity = HighestVelocity(&x, &y, iT_Peak,  iP_On);

			//wyznaczenie koncow zalamka T przy pomocy metod: paraboli oraz stycznej
			T_EndP[j]=CalculateTendParabol(&x, &y, highestvelocity, iT_Peak, iP_On);
			T_EndT[j]=CalculateTendTangent(&x, &y, highestvelocity, iT_Peak, iP_On);			
		}
	EvaluateQTDisp();
}

void QT_DISP::Filtering(vector <double> *y, int QRS_End, int P_On)
{
	vector <double> wynik(y->begin() + QRS_End, y->begin() + P_On);
	vector <double> yTemp(y->begin() + QRS_End, y->begin() + P_On);

	wynik[0] = (yTemp[0]+yTemp[1])/2;
	wynik[1] = (yTemp[0]+yTemp[1]+yTemp[2])/3;         
	for (unsigned int i=5; i< wynik.size(); i++)
	{
		wynik[i-3] =  (yTemp[i]  + yTemp[i-1] + yTemp[i-2] + yTemp[i-3] +yTemp[i-4]+yTemp[i-5])/6;
	}
    wynik[wynik.size() -3] = (yTemp[yTemp.size()-1]+yTemp[yTemp.size()-2]+yTemp[yTemp.size()-3]+yTemp[yTemp.size()-4])/4;
	wynik[wynik.size() -2] = (yTemp[yTemp.size()-1]+yTemp[yTemp.size()-2]+yTemp[yTemp.size()-3])/3;
	wynik[wynik.size() -1] = (yTemp[yTemp.size()-1]+yTemp[yTemp.size()-2])/2;

	y->erase(y->begin() + QRS_End, y->begin() + P_On);
	y->insert(y->begin() + QRS_End, wynik.begin(), wynik.end());
}

int QT_DISP::FindTPeak(vector<double> y, int QRS_End, int P_On)
{
	double maxValue = 0;
	int maxPlace = 0;

	for(int i = QRS_End; i < (P_On - 20); i++)
	{
		if (maxValue < y[i])
		{
			maxValue = y[i];
			maxPlace = i;
		}
	}
	return maxPlace;
}

int QT_DISP::HighestVelocity(vector <double> *x, vector <double> *y, int TPeak, int P_On)
	//actually, lowest velocity, because it is negative
{
	double highestValue=0;
	int highestPlace=0;

	for (int i = TPeak; i < (P_On - 5); i++)
	{
		if( (y->at(i+1) - y->at(i)) / (x->at(i+1) - x->at(i)) < highestValue)
		{
			highestValue = (y->at(i+1) - y->at(i)) / (x->at(i+1) - x->at(i));
			highestPlace = i;
		}
	}
	return highestPlace;
}

void QT_DISP::Tangent(double* a, double* b, vector <double> *x, vector <double> *y,int highestVelocityPoint)
{
	double highestVelocityPointValue = y->at(highestVelocityPoint);

	double differentialValue = (y->at(highestVelocityPoint + 1) - y->at(highestVelocityPoint)) / (x->at(highestVelocityPoint + 1) - x->at(highestVelocityPoint));

	(*a) = differentialValue;
	(*b) = highestVelocityPointValue - differentialValue * x->at(highestVelocityPoint);
}

double QT_DISP::CalculateTendTangent(vector <double> *x, vector <double> *y, int highestVelocity, int T_Peak, int P_Onset)
{
	double a, b;
	Tangent(&a,&b, x, y, highestVelocity);

	double point0OfTangent;
	double distancePeakTangent = (y->at(T_Peak) - b)/a - x->at(T_Peak); 

	point0OfTangent = (950 - b) / a;		//!!!!!!!!!!!!!!!!!!!!!!!!!!! isoline is set to const 950 for now!!!!!!!!!!!!

	if(x->at(P_Onset) < point0OfTangent + distancePeakTangent)
		return x->at(P_Onset);
	else
		return point0OfTangent + distancePeakTangent;	
}

double QT_DISP::CalculateTendParabol(vector <double> *x, vector <double> *y, int highestVelocity,  int T_Peak, int P_Onset)
{
	double a=0;
	double b=0;
	double c=0;

	double error = 1.0e18;
	double errorTemp = 0;
	double aTemp = a;
	double bTemp = b;
	double cTemp = c;
	double aBest = a;
	double bBest = b;
	double cBest = c;

	for (double i = 0; i<1000; ++i)
	{
		aTemp = i;
		for (int j = 0; j < 10; j++)
		{
			bTemp = -2*aTemp*x->at(P_Onset) + j * (-2*aTemp*x->at(T_Peak) + 2*aTemp*x->at(P_Onset)) /100;
			cTemp = y->at(highestVelocity) - aTemp*x->at(highestVelocity)*x->at(highestVelocity) - bTemp*x->at(highestVelocity);
			errorTemp = 0.0;
			for (double m = highestVelocity; m < (highestVelocity+P_Onset)/2 ; m++)
			{
				errorTemp = errorTemp + (y->at(m) - (aTemp*x->at(m)*x->at(m) + bTemp*x->at(m) + cTemp)) * (y->at(m) - (aTemp*x->at(m)*x->at(m) + bTemp*x->at(m) + cTemp));
			}
				if (errorTemp < error)
				{
					aBest = aTemp;
					bBest = bTemp;
					cBest = cTemp;
					error = errorTemp;
				}
			}
		}
		a = aBest;
		b = bBest;
		c = cBest;

		for(int i = -10; i<10; i++)
		{
			aTemp = a + i * a  / 100;
			for (int j = -10; j<10; j++)
			{
				bTemp = b + b * j / 10;
				for (int k = -10; k<10; k++)
				{
					cTemp = c + c * k / 10;
					errorTemp = 0.0;
					for (double m = highestVelocity; m < (highestVelocity+P_Onset)/2; m++)
					{
						errorTemp = errorTemp + (y->at(m) - (aTemp*x->at(m)*x->at(m) + bTemp*x->at(m) + cTemp)) * (y->at(m) - (aTemp*x->at(m)*x->at(m) + bTemp*x->at(m) + cTemp));

					}
					if (errorTemp < error)
					{
						aBest = aTemp;
						bBest = bTemp;
						cBest = cTemp;
						error = errorTemp;
					}					
				}
			}
		}
		a = aBest;
		b = bBest;
		c = cBest;

		if(x->at(P_Onset) < -bBest/(2*aBest))
			return x->at(P_Onset);
		else
			return -bBest/(2*aBest);
}

void QT_DISP::EvaluateQTDisp()
{
		heartAction = (60 * samplingFrequency * QRS_On.size() / signals2.size() ) ;

		for(int j =0; j< T_EndP.size(); j++)
		{
			double RR = 60/heartAction;
			double gapQT_T = 1000*(T_EndT[j]-QRS_On[j]/samplingFrequency);
			double gapQT_P = 1000*(T_EndP[j]-QRS_On[j]/samplingFrequency);

			int EvaluatedValue;

			for (int k = 0; k<8;k++)
			{
				switch(k)
				{
					case 0:
						EvaluatedValue = EvaluateBazzet(gapQT_T, RR);
						break;
    
					case 1:
						EvaluatedValue = EvaluateFrideric(gapQT_T, RR);
						break;
   					case 2:
						EvaluatedValue = EvaluateHodges(gapQT_T, heartAction);
						break;
    
					case 3:
						EvaluatedValue = EvaluateFramingham(gapQT_T, RR);
						break;

					case 4:
						EvaluatedValue = EvaluateBazzet(gapQT_P, RR);
						break;
    
					case 5:
						EvaluatedValue = EvaluateFrideric(gapQT_P, RR);
						break;
   					case 6:
						EvaluatedValue = EvaluateHodges(gapQT_P, heartAction);
						break;
    
					case 7:
						EvaluatedValue = EvaluateFramingham(gapQT_P, RR);
						break;
				}

				if (EvaluatedValue == 0)
					evaluations[k].numberOfCorrectQT++;
				else if (EvaluatedValue == 1)
					evaluations[k].numberOfTooLowQT++;
				else
					evaluations[k].numberOfTooHighQT++;
			}
		}
}

int QT_DISP::EvaluateBazzet(double gapQT, double RR)
{
	//wyliczenie ze wzoru Bazzeta i ocena wyliczenia
	double BazzetValue =  gapQT/sqrt(RR);

	//interpretacja wyniku ze wzoru Bazzeta : wynik poprawny (405-452 ms)
	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
	
	int BazzetState = 0;

	if (BazzetValue < 405)
	{
		BazzetState = 1;
	}

	if (BazzetValue  > 452)
	{
		BazzetState = 2;
	}

	return BazzetState;
}

int QT_DISP::EvaluateFrideric(double gapQT, double RR)
{
	double FridericValue =  gapQT/pow(RR, 1/3);
	//interpretacja wyniku ze wzoru Friderica : wynik poprawny (386-432 ms)
	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o

	int FridericState = 0;

	if (FridericValue < 386)
	{
		FridericState = 1;
	}

	if (FridericValue > 432)
	{
		FridericState = 2;
	}

	return FridericState;
}


int QT_DISP::EvaluateHodges(double gapQT, double heartAction)
{
	//wyliczenie ze wzoru Hodges
	double HodgesValue =  gapQT + 1.75 * (heartAction - 60); 

	//interpretacja wyniku ze wzoru Hodges : wynik poprawny (390-432 ms)
	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
	
	int HodgesState = 0;

	if (HodgesValue < 390)
	{
		HodgesState = 1;
	}

	if (HodgesValue > 432)
	{
		HodgesState = 2;
	}

	return HodgesState;
}
int QT_DISP::EvaluateFramingham(double gapQT, double RR)
{
	//wyliczenie ze wzoru Framingham
	double FraminghamValue =  gapQT + 0.154* (1 - RR); 

	//interpretacja wyniku ze wzoru Framingham : wynik poprawny (388-432 ms)
	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o

	int FraminghamState = 0;

	if (FraminghamValue < 388)
	{
		FraminghamState = 1;
	}

	if (FraminghamValue > 432)
	{
		FraminghamState = 2;
	}

	return FraminghamState;

}

//int main(
//	
//{
//
//    int channels = signals2.size() - 1;
//    
//	//ponizej, zakomentowany znajduje sie sposob zadeklarowania wektorow potrzebnych do przeslania danych : sama skladnia oraz rozmiary, jakie beda nam potrzebne
//	/*
//	const vector <int> temp(QRS_On.size()-1,0);
//
//	vector <vector <int>> tangentEvaluationB(channels,temp);
//	vector <vector <int>> tangentEvaluationFrc(channels,temp);
//	vector <vector <int>> tangentEvaluationH(channels,temp);
//	vector <vector <int>> tangentEvaluationFhm(channels,temp);
//
//	vector <vector <int>> parabolEvaluationB(channels,temp);
//	vector <vector <int>> parabolEvaluationFrc(channels,temp);
//	vector <vector <int>> parabolEvaluationH(channels,temp);
//	vector <vector <int>> parabolEvaluationFhm(channels,temp);
//	*/
//
//	int rightBazzetTangent=0;
//	int rightFridericTangent=0;
//	int rightHodgesTangent=0;
//	int rightFraminghamTangent=0;
//
//	int rightBazzetParabol=0;
//	int rightFridericParabol=0;
//	int rightHodgesParabol=0;
//	int rightFraminghamParabol=0;
//
//	int lowBazzetTangent=0;
//	int lowFridericTangent=0;
//	int lowHodgesTangent=0;
//	int lowFraminghamTangent=0;
//
//	int lowBazzetParabol=0;
//	int lowFridericParabol=0;
//	int lowHodgesParabol=0;
//	int lowFraminghamParabol=0;
//
//	int highBazzetTangent=0;
//	int highFridericTangent=0;
//	int highHodgesTangent=0;
//	int highFraminghamTangent=0;
//
//	int highBazzetParabol=0;
//	int highFridericParabol=0;
//	int highHodgesParabol=0;
//	int highFraminghamParabol=0;
//
	//for(int i = 0; i < channels;  ++i)
 //   {
 //       for(int j = 0; j < qrs_on.size() - 1; ++j)
 //       {
 //           vector <double> x (signals2[0].begin() + qrs_on[i][j], signals2[0].begin() + qrs_on[i][j + 1]);
 //           vector <double> y (signals2[i+1].begin() + qrs_on[i][j], signals2[i+1].begin() + qrs_on[i][j + 1]);

 //           int iqrs_on = qrs_on[i][j];
 //           int iqrs_end = qrs_end[i][j];
 //           int ip_on = p_on [i][j];

	//		vector <double> xqrsend_pon (signals2[0].begin() +iqrs_end, signals2[0].begin() + ip_on);
	//		vector <double> yqrsend_pon (signals2[i+1].begin() + iqrs_end, signals2[i+1].begin() + ip_on);


	//		filtering(xqrsend_pon,yqrsend_pon);
	//		int tpeak = findtpeak(xqrsend_pon,yqrsend_pon,iqrs_end,ip_on); 
	//		int highestvelocity = highestvelocity(xqrsend_pon,yqrsend_pon,tpeak, ip_on);

	//		double a,b,c;
	//		double vertex = polifitting(&a,&b,&c,xqrsend_pon,yqrsend_pon,tpeak,ip_on);

	//		double tend =  findtend_tangent(xqrsend_pon,yqrsend_pon,highestvelocity,tpeak);

	//		double heartaction = x.back() / qrs_on[0].size();

	//		int bazzetstatetangent, fridericstatetangent, hodgesstatetangent, framinghamstatetangent;
	//		int bazzetstateparabol, fridericstateparabol, hodgesstateparabol, framinghamstateparabol;

	//		double bazzetvaluetangent, fridericvaluetangent, hodgesvaluetangent, framinghamvaluetangent;
	//		double bazzetvalueparabol, fridericvalueparabol, hodgesvalueparabol, framinghamvalueparabol;

	//		double gapqttangent = iqrs_on - tend;
	//		double gapqtparabol = iqrs_on - vertex;
	//		
	//		dispersionevaluation (gapqttangent, heartaction, &bazzetstatetangent, &fridericstatetangent, &hodgesstatetangent, &framinghamstatetangent, &bazzetvaluetangent, &fridericvaluetangent, &hodgesvaluetangent, &framinghamvaluetangent);
	//		dispersionevaluation (gapqtparabol, heartaction, &bazzetstateparabol, &fridericstateparabol, &hodgesstateparabol, &framinghamstateparabol, &bazzetvalueparabol, &fridericvalueparabol, &hodgesvalueparabol, &framinghamvalueparabol);

	//		tangentevaluationb[i][j]   = bazzetstatetangent;
	//		tangentevaluationfrc[i][j] = fridericstatetangent;
	//		tangentevaluationh[i][j]   = hodgesstatetangent;
	//		tangentevaluationfhm[i][j] = framinghamstatetangent;

	//		parabolevaluationb[i][j]   = bazzetstateparabol;
	//		parabolevaluationfrc[i][j] = fridericstateparabol;
	//		parabolevaluationh[i][j]   = hodgesstateparabol;
	//		parabolevaluationfhm[i][j] = framinghamstateparabol;


//			switch( BazzetStateTangent )
//			{
//			case 0:
//				rightBazzetTangent++;
//				break;
//			case 1:
//				lowBazzetTangent++;
//				break;
//			case 2:
//				highBazzetTangent++;
//				break;
//			}
//
//			switch( FridericStateTangent )
//			{
//			case 0:
//				rightFridericTangent++;
//				break;
//			case 1:
//				lowFridericTangent++;
//				break;
//			case 2:
//				highFridericTangent++;
//				break;
//			}
//
//			switch( HodgesStateTangent )
//			{
//			case 0:
//				rightHodgesTangent++;
//				break;
//			case 1:
//				lowHodgesTangent++;
//				break;
//			case 2:
//				highHodgesTangent++;
//				break;
//			}
//
//			switch( FraminghamStateTangent )
//			{
//			case 0:
//				rightFraminghamTangent++;
//				break;
//			case 1:
//				lowFraminghamTangent++;
//				break;
//			case 2:
//				highFraminghamTangent++;
//				break;
//			}
//
//			switch( BazzetStateParabol )
//			{
//			case 0:
//				rightBazzetParabol++;
//				break;
//			case 1:
//				lowBazzetParabol++;
//				break;
//			case 2:
//				highBazzetParabol++;
//				break;
//			}
//
//			switch( FridericStateParabol )
//			{
//			case 0:
//				rightFridericParabol++;
//				break;
//			case 1:
//				lowFridericParabol++;
//				break;
//			case 2:
//				highFridericParabol++;
//				break;
//			}
//
//			switch( HodgesStateParabol )
//			{
//			case 0:
//				rightHodgesParabol++;
//				break;
//			case 1:
//				lowHodgesParabol++;
//				break;
//			case 2:
//				highHodgesParabol++;
//				break;
//			}
//
//			switch( FraminghamStateParabol )
//			{
//			case 0:
//				rightFraminghamParabol++;
//				break;
//			case 1:
//				lowFraminghamParabol++;
//				break;
//			case 2:
//				highFraminghamParabol++;
//				break;
//			}
//
//        }
//					
//    }
//
//
//}
///*
//int _tmain(int argc, _TCHAR* argv[])
//{
//	double a,b,c;
//	int size = 301;
//
//	double x[301];
//	double y[301];
//
//	x[0] = 2;
//	y[0] = cos(M_PI*x[0]) / (-log(x[0]));
//
//	for (int i=1;i<size;i++)
//	{
//		x[i] = x[i-1] +0.01;
//		y[i] = cos(M_PI*x[i]) / (-log(x[i]))+1;
//
//		//cout << " y[" << i <<"] :"<< y[i] << endl; 
//	}
//
//	int biggestVelocity = BiggestVelocity (x,y,size);
//
//	cout << "Najwiekszy spadek predkosci :" << biggestVelocity<<endl;
//
//	Tangent(&a, &b, x, y,size, biggestVelocity);
//	cout << "a: " << a << endl << "b: " << b << endl;
//
//	//double cone;
//	//cone = poliFitting(&a,&b,&c,x,y,size);
//
//	//cout << "a: " << a << endl << "b: " << b << endl << "c: " << c << endl << endl << "cone: " << cone; 
//	double tend = FindTEnd_Tangent(x, y, size);
//
//	cout << "Koniec zalamka :" << tend << endl;
//	cin >> a;
//}*/
//
//void DispersionEvaluation ( double gapQT, double heartAction, 
//	int* BazzetState, int* FridericState, int* HodgesState, int* FraminghamState, 
//	double* BazzetValue, double* FridericValue, double* HodgesValue, double* FraminghamValue)
//{
//
//	//Zbadanie dlugosci trwania odstepu QT. 
//
//	double RR = 60/heartAction;
//
//	BazzetState = 0;
//	FridericState = 0;
//	HodgesState = 0;
//	FraminghamState = 0;
//
//	//wdobrymrytmie.pl/archiwum/36-styczen-2008/117-zespoy-wyduonego-i-krotkiego-qt-dwie-grone-skrajnoci
//	//wyliczenie ze wzoru Bazzeta i ocena wyliczenia
//	*BazzetValue =  gapQT/sqrt(RR);
//
//	//interpretacja wyniku ze wzoru Bazzeta : wynik poprawny (405-452 ms)
//	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
//	
//	if (*BazzetValue < 405)
//	{
//		*BazzetState = 1;
//	}
//
//	if (*BazzetValue  > 452)
//	{
//		*BazzetState = 2;
//	}
//
//	//wyliczenie ze wzoru Friderica
//	*FridericValue =  gapQT/pow(RR, 1/3);
//
//	//interpretacja wyniku ze wzoru Friderica : wynik poprawny (386-432 ms)
//	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
//	
//	if (*FridericValue < 386)
//	{
//		*FridericState = 1;
//	}
//
//	if (*FridericValue > 432)
//	{
//		*FridericState = 2;
//	}
//
//	//wyliczenie ze wzoru Hodges
//	*HodgesValue =  gapQT + 1.75 * (heartAction - 60); 
//
//	//interpretacja wyniku ze wzoru Hodges : wynik poprawny (390-432 ms)
//	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
//	
//	if (*HodgesValue < 390)
//	{
//		*HodgesState = 1;
//	}
//
//	if (*HodgesValue > 432)
//	{
//		*HodgesState = 2;
//	}
//
//	//wyliczenie ze wzoru Framingham
//	*FraminghamValue =  gapQT + 0.154* (1 - RR); 
//
//	//interpretacja wyniku ze wzoru Framingham : wynik poprawny (388-432 ms)
//	//0 - w porz¹dku, 1 - za ma³o, 2 - za du¿o
//
//	if (*FraminghamValue < 388)
//	{
//		*FraminghamState = 1;
//	}
//
//	if (*FraminghamValue > 432)
//	{
//		*FraminghamState = 2;
//	}
// 
//}
//
//int Filtering(vector <double> x, vector <double> y)
//{
//
//	if (y.size()!=x.size())
//	{
//		cout << "Blad: Dlugosci Wektorow x oraz y sa niezgodne" << endl;
//		return -1;
//	}
//
//	vector <double> bufor[x.size()];
//	vector <double> wynik[y.size()];
//
//	bufor[0] = y[0];
//	bufor[1] = y[1];
//	bufor[2] = y[2];
//	bufor[3] = y[3];
//	bufor[4] = y[4];
//
//	wynik[0] = (y[0]+y[1])/2;
//	wynik[1] = (y[0]+y[1]+y[2])/3;
//         
//	for (int i=5; i< x.size(); i++)
//	{
//	  bufor[i] = y[i];
//	  wynik[i-3] =  (y[i]  + y[i-1] + y[i-2] + y[i-3] +y[i-4]+y[i-5])/6;
//	}
//
//	
//    wynik[x.size() -3] = (y[x.size()-1]+y[x.size()-2]+y[x.size()-3]+y[x.size()-4])/4;
//	wynik[x.size() -2] = (y[x.size()-1]+y[x.size()-2]+y[x.size()-3])/3;
//	wynik[x.size() -1] = (y[x.size()-1]+y[x.size()-2])/2;
//	
//	y = wynik;
//
//	return 0;
//}
//
//int HighestVelocity(vector <double> x, vector <double> y, int size, int TPeak, int P_Onset)
//{
//	double previous;
//	double current;
//
//	double highestValue=0;
//	int highestPlace=0;
//
//	for (int i=TPeak; i < P_Onset; i++)
//	{
//		previous = y[i];
//		current = y[i+1];
//
//		if( (previous - current) > biggestValue)
//		{
//			highestValue = previous - current;
//			highestPlace = i;
//		}
//	}
//
//	return highestPlace;
//}
//
//void Tangent(double* a, double* b, vector <double> x, vector <double> y,int highestVelocityPoint)
//{
//	double highestVelocityPointX, highestVelocityPointY;
//
//	highestVelocityPointX = x[highestVelocityPoint];
//	highestVelocityPointY = y[highestVelocityPoint];
//
//	double differentialValue = -(y[highestVelocityPoint] - y[highestVelocityPoint+1])/(x[1]-x[0]);
//
//	(*a) = differentialValue;
//	(*b) = highestVelocityPointY - differentialValue * highestVelocityPointX;
//}
//
////pobiera od maksimum zalamka T do koñca sygnalu
//double FindTEnd_Tangent(vector <double> x, vector <double> y, int highestVelocity, int tPeak)
//{
//	
//	if (y.size()!=x.size())
//	{
//		cout << "Blad: Dlugosci Wektorow x oraz y sa niezgodne" << endl;
//		return -1;
//	}
//
//	double a, b;
//	Tangent(&a,&b, x, y, highestVelocity);
//
//	double distancePeakTangent;
//	double point0OfTangent;
//
//	//cout << "lokalizacja peaka: "<< x[peakLoc] << endl;
//
//	distancePeakTangent = (y[tPeak] - b)/a - x[tPeak]; 
//	point0OfTangent = -(b / a);
//
//	//cout << "DistancePeakTangent: " << distancePeakTangent;
//	//cout << "point0ofTangent: " << point0OfTangent;
//
//	return point0OfTangent + distancePeakTangent;
//
//}
//
//int FindTPeak(vector<double> x, vector<double> y, int QRS_End, int P_Onset)
//{
//	double maxValue = 0;
//	int maxPlace = 0;
//
//	for(int i = QRS_End; i<P_Onset;i++)
//	{
//		if (maxValue < y[i])
//		{
//			maxValue = y[i];
//			maxPlace = i;
//		}
//	}
//
//	return maxPlace;
//}
//
//double poliFitting(double* a, double* b, double* c, vector <double> x, vector <double> y, int highestVelocity, int P_Onset)
//{
//	if (y.size()!=x.size())
//	{
//		cout << "Blad: Dlugosci Wektorow x oraz y sa niezgodne" << endl;
//		return -1;
//	}
//
//	double error = 1.0e6, errorTemp;
//	double aTemp, bTemp, cTemp, aBest, bBest, cBest;
//
//
//	a=0;
//	b=0;
//	c=0;
//
//	aTemp = a;
//	bTemp = b;
//	cTemp = c;
//	aBest = a;
//	bBest = b;
//	cBest = c;
//
//	for(int i = 0; i <= 10; i=i+0.1)
//	{
//		aTemp = i;
//		for(int j = 0; j <= 100; j++)
//		{
//			
//			bTemp = -2*aTemp*x(POn) + j * (-2*aTemp*x(Tp) + 2*aTemp*x(POn))bTemp = (*b) + j * (*b)/100;
//			for(int k = -100; k <= 100; k++)
//			{
//				cTemp = y(POn) + bTemp*bTemp/(4*aTemp) + k * (y(Tp) - y(POn)) /100;
//				errorTemp = 0.0;
//				for(int m = highestVelocity; m < (highestVelocity+P_Onset)/2; m++)
//				{
//					errorTemp += pow((y[m] - (aTemp*x[m]*x[m] + bTemp*x[m] + cTemp)), 2);
//				}
//				if(errorTemp < error)
//				{
//					aBest = aTemp;
//					bBest = bTemp;
//					cBest = cTemp;
//					error = errorTemp;
//				}
//			}
//		}
//	}
//
//	(*a) = aBest;
//	(*b) = bBest;
//	(*c) = cBest;
//
//	for(int i = -10; i <= 10; i++)
//	{
//		aTemp = (*a) + i * (*a)/10;
//		for(int j = -10; j <= 10; j++)
//		{
//			bTemp = (*b) + j * (*b)/10;
//			for(int k = -10; k <= 10; k++)
//			{
//				cTemp = (*c) + k * (*c)/10;
//				errorTemp = 0.0;
//				for(int m = highestVelocity; m < (highestVelocity+P_Onset)/2; m++)
//				{
//					errorTemp += pow((y[m] - (aTemp*x[m]*x[m] + bTemp*x[m] + cTemp)), 2);
//				}
//				if(errorTemp < error)
//				{
//					aBest = aTemp;
//					bBest = bTemp;
//					cBest = cTemp;
//					error = errorTemp;
//				}
//			}
//		}
//	}
//	
//	(*a) = aBest;
//	(*b) = bBest;
//	(*c) = cBest;
//
//	return -bBest/(2*aBest);
//
//}