#include <string>
using namespace std;

class Evaluation
{
public:
	string nameOfEvaluation;
	int numberOfCorrectQT;
	int numberOfTooLowQT;
	int numberOfTooHighQT;

	Evaluation()
	{
		numberOfCorrectQT = 0;
		numberOfTooLowQT = 0;
		numberOfTooHighQT = 0;
	};
	Evaluation(string name)
	{
		nameOfEvaluation = name;
		numberOfCorrectQT = 0;
		numberOfTooLowQT = 0;
		numberOfTooHighQT = 0;
	};
};

