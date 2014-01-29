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
	if(QRS_End[0] < QRS_On[0])
	{
		QRS_End.erase(QRS_End.begin());
	}
	P_On = in_P_On;
	samplingFrequency = in_samplingFrequency;

	heartBeats = QRS_On.size() - 1;

	/*channels = signals2.size() - 1;
	T_Peak.resize(channels);
	T_EndP.resize(channels);
	T_EndT.resize(channels);
	*/
	T_Peak.resize(heartBeats);
	T_EndT.resize(heartBeats);
	T_EndP.resize(heartBeats);
	QTT.resize(heartBeats);
	QTP.resize(heartBeats);
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
	
		int QRS_On_len, QRS_End_len, P_On_len;

		file >> QRS_On_len;
		QRS_On = vector <int>(QRS_On_len, 0);
		heartBeats = QRS_On.size() - 1;
		for(int i = 0; i < QRS_On_len; ++i)
		{
			file >> QRS_On[i];
		}

		file >> QRS_End_len;
		QRS_End = vector <int>(QRS_End_len, 0);
		for(int i = 0; i < QRS_End_len; ++i)
		{
			file >> QRS_End[i];
		}

		file >> P_On_len;
		P_On = vector <int>(P_On_len, 0);
		for(int i = 0; i < P_On_len; ++i)
		{
			file >> P_On[i];
		};
		T_Peak.resize(heartBeats);
		T_EndT.resize(heartBeats);
		T_EndP.resize(heartBeats);
		QTT.resize(heartBeats);
		QTP.resize(heartBeats);

		file.close();
	}
}
	
// Funkcja wyjsciowa - konce zalamka T
vector <double> QT_DISP::returnTEnd()
{
	return T_EndT;				//!!!!!!!!!!!!!!!!!!!!!!!! na razie zwracane tylko paraboliczne, jakiœ check box?
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

void QT_DISP::setOutput(vector <Evaluation> &out_evaluations, vector <double> &T_End)
{

	//out_evaluations = evaluations;
	out_evaluations = evaluations;
    T_End = T_EndT;
	
}

void QT_DISP::Run()
{
    for(int j = 0; j < heartBeats; ++j)
    {			
        //cout << j << endl;
        int iQRS_On = QRS_On[j] - QRS_On[j];
		int	iQRS_End = QRS_End[j] - QRS_On[j];
		if(iQRS_End < 0)
		{
			continue;
		}
		int	iP_On = P_On[j] - QRS_On[j];	
		if(P_On[j] > QRS_On[j + 1])
		{
			iP_On = iQRS_End + (int) (0.2 * (QRS_On[j + 1] - QRS_End[j]));
			P_On.insert(P_On.begin() + j, 0.0);
		}
		if(iP_On < iQRS_End)
		{
			continue;
		}
            
		vector <double> *y = new vector <double> (signals2.begin() + QRS_On[j], signals2.begin() + QRS_On[j + 1]);
		vector <double> *x = new vector <double> (QRS_On[j + 1]-QRS_On[j], 0);
		int iter = 0;
		for (int i = QRS_On[j]; i < QRS_On[j + 1]; i++, iter++)
		{
			x->at(iter) = i/samplingFrequency;
		}

		//filtracja
		Filtering(y, iQRS_End, iP_On);
		Filtering(y, iQRS_End, iP_On);
		Filtering(y, iQRS_End, iP_On);
		Filtering(y, iQRS_End, iP_On);

		//odnalezienie potrzebnych do wyszukania konca zalamka miejsc dodatkowych
		int iT_Peak = FindTPeak(y,iQRS_End,iP_On); 
		T_Peak[j] = iT_Peak + QRS_On[j];
		int highestvelocity = HighestVelocity(x, y, iT_Peak,  iP_On);

		//wyznaczenie koncow zalamka T przy pomocy metod: paraboli oraz stycznej
		T_EndP[j]=CalculateTendParabol(x, y, highestvelocity, iT_Peak, iP_On);
		T_EndT[j]=CalculateTendTangent(x, y, highestvelocity, iT_Peak, iP_On);

		//i obliczenie d³ugoœci odcinka QT
		CalculateQT(x->at(0), j);

		//i ocena tego odcinka
		EvaluateQTDisp(QTT[j], QTP[j]);
			
		delete x;
		delete y;
	}
	CalculateStatistics();
}

void QT_DISP::CalculateStatistics()
{
	for(int i = 0; i < 8; ++i)
	{
		if(i == 0)
		{
			evaluations[i].StatisticalEvaluation(&QTT, 0.0, 0.0);
		}
		else if(i < 4)
		{
			evaluations[i].StatisticalEvaluation(&QTT, evaluations[0].averageQT, evaluations[0].standardDeviationQT);
		}
		else if(i == 4)
		{
			evaluations[i].StatisticalEvaluation(&QTP, 0.0, 0.0);
		}
		else
		{
			evaluations[i].StatisticalEvaluation(&QTP, evaluations[4].averageQT, evaluations[4].standardDeviationQT);
		}
	}
}

void QT_DISP::CalculateQT(double QRS_OnTime, int number_T_End_QT)
{
	QTP[number_T_End_QT] = T_EndP[number_T_End_QT] - QRS_OnTime;
	QTT[number_T_End_QT] = T_EndT[number_T_End_QT] - QRS_OnTime;
    //cout << QTP[number_T_End_QT] << endl;
    //cout << QTT[number_T_End_QT] << endl;
}

void QT_DISP::Filtering(vector <double> *y, int QRS_End, int P_On)
{
	if(P_On - QRS_End < 4)
	{
		P_On += (4 - (P_On - QRS_End));
	}
	vector <double> *wynik = new vector <double>(y->begin() + QRS_End, y->begin() + P_On);
	vector <double> *yTemp = new vector <double>(y->begin() + QRS_End, y->begin() + P_On);

	wynik->at(0) = (yTemp->at(0)+yTemp->at(1))/2;
	wynik->at(1) = (yTemp->at(0)+yTemp->at(1)+yTemp->at(2))/3;         
	for (unsigned int i=5; i< wynik->size(); i++)
	{
		wynik->at(i-3) =  (yTemp->at(i)  + yTemp->at(i-1) + yTemp->at(i-2) + yTemp->at(i-3) +yTemp->at(i-4)+yTemp->at(i-5))/6;
	}
    wynik->at(wynik->size() -3) = (yTemp->at(yTemp->size()-1)+yTemp->at(yTemp->size()-2)+yTemp->at(yTemp->size()-3)+yTemp->at(yTemp->size()-4))/4;
	wynik->at(wynik->size() -2) = (yTemp->at(yTemp->size()-1)+yTemp->at(yTemp->size()-2)+yTemp->at(yTemp->size()-3))/3;
	wynik->at(wynik->size() -1) = (yTemp->at(yTemp->size()-1)+yTemp->at(yTemp->size()-2))/2;

	y->erase(y->begin() + QRS_End, y->begin() + P_On);
	y->insert(y->begin() + QRS_End, wynik->begin(), wynik->end());

	delete wynik;
	delete yTemp;
}

int QT_DISP::FindTPeak(vector<double> *y, int QRS_End, int P_On)
{
	double maxValue = 0;
	int maxPlace = 0;

	for(int i = QRS_End; i < (P_On - (0.1 * (P_On - QRS_End))); i++)
	{
		if (maxValue < y->at(i))
		{
			maxValue = y->at(i);
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

	for (int i = TPeak; i < (P_On - (0.1 * (P_On - TPeak))); i++)
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

	point0OfTangent = (0 - b) / a;		// isoline is set to const 0 for now

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

		if(aBest == 0)
            return x->at(P_Onset);
		if(x->at(P_Onset) < -bBest/(2*aBest))
			return x->at(P_Onset);
		else
			return -bBest/(2*aBest);
}

void QT_DISP::EvaluateQTDisp(double QTT, double QTP)
{
	double RR = 60/heartAction;
	double gapQT_T = 1000*(QTT);
	double gapQT_P = 1000*(QTP);

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
	double FridericValue =  gapQT/pow(RR, 1.0/3.0);
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
