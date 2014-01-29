#ifndef EVALUATION_H
#define EVALUATION_H

#include <Qstring>
using namespace std;

class Evaluation
{
public:
    QString nameOfEvaluation;
	int numberOfCorrectQT;
	int numberOfTooLowQT;
	int numberOfTooHighQT;
	double percentOfCorrectQT;
	double percentOfTooLowQT;
	double percentOfTooHighQT;
	double averageQT;
	double standardDeviationQT;
	
	Evaluation()
	{
		numberOfCorrectQT = 0;
		numberOfTooLowQT = 0;
		numberOfTooHighQT = 0;
		percentOfCorrectQT = 0;
		percentOfTooLowQT = 0;
		percentOfTooHighQT = 0;
		averageQT = 0;
		standardDeviationQT = 0;
	};
	
    Evaluation(QString name)
	{
		nameOfEvaluation = name;
		numberOfCorrectQT = 0;
		numberOfTooLowQT = 0;
		numberOfTooHighQT = 0;
		percentOfCorrectQT = 0;
		percentOfTooLowQT = 0;
		percentOfTooHighQT = 0;
		averageQT = 0;
		standardDeviationQT = 0;
	};
	
	void CalculatePercentage()
	{
		int sum = numberOfCorrectQT + numberOfTooLowQT + numberOfTooHighQT;
		percentOfCorrectQT = numberOfCorrectQT / sum;
		percentOfTooLowQT = numberOfTooLowQT / sum;
		percentOfTooHighQT = numberOfTooHighQT / sum;
        //cout << percentOfCorrectQT << endl;
        //cout << percentOfTooLowQT << endl;
        //cout << percentOfTooHighQT << endl;
	}

	void CalculateAverage(vector <double> *QT)
	{
		double sum = 0;
		for(unsigned int i = 0; i < QT->size(); ++i)
		{
			sum += QT->at(i);
		}
		averageQT = sum / QT->size();
        //cout << averageQT << endl;
	}

	void CalculateStandardDeviation(vector <double> *QT)
	{
		double sumSquaredDiff = 0;
		for(unsigned int i = 0; i < QT->size(); ++i)
		{
			sumSquaredDiff += (QT->at(i) - averageQT) * (QT->at(i) - averageQT);
		}
		standardDeviationQT = sqrt(sumSquaredDiff / (QT->size() - 1));
        //cout << standardDeviationQT << endl;
	}
	
	void StatisticalEvaluation(vector <double> *QT, double avg, double stdDev)
	{
		CalculatePercentage();
		if(avg == 0)
		{
			CalculateAverage(QT);
		}
		else
		{
			averageQT = avg;
		}
		if(stdDev == 0)
		{
			CalculateStandardDeviation(QT);
		}
		else
		{
			standardDeviationQT = stdDev;
		}
	}
};

#endif
