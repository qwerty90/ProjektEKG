int main(
	/*dane wejsciowe*/		vector <vector <double>> signals, vector <vector <int>> QRS_On, vector <vector <int>> QRS_End, vector <vector <int>> P_On,
	/*dane wyjsciowe*/		vector <vector <int>> tangentEvaluationB,vector <vector <int>> tangentEvaluationFrc, vector <vector <int>> tangentEvaluationH,
	/*dane wyjsciowe*/		vector <vector <int>> tangentEvaluationFhm, vector <vector <int>> parabolEvaluationB, vector <vector <int>> parabolEvaluationFrc,
	/*dane wyjsciowe*/		vector <vector <int>> parabolEvaluationH, vector <vector <int>> parabolEvaluationFhm)
	
{

    int channels = signals.size() - 1;
    
	//ponizej, zakomentowany znajduje sie sposob zadeklarowania wektorow potrzebnych do przeslania danych : sama skladnia oraz rozmiary, jakie beda nam potrzebne
	/*
	const vector <int> temp(QRS_On.size()-1,0);

	vector <vector <int>> tangentEvaluationB(channels,temp);
	vector <vector <int>> tangentEvaluationFrc(channels,temp);
	vector <vector <int>> tangentEvaluationH(channels,temp);
	vector <vector <int>> tangentEvaluationFhm(channels,temp);

	vector <vector <int>> parabolEvaluationB(channels,temp);
	vector <vector <int>> parabolEvaluationFrc(channels,temp);
	vector <vector <int>> parabolEvaluationH(channels,temp);
	vector <vector <int>> parabolEvaluationFhm(channels,temp);
	*/

	for(int i = 0; i < channels;  ++i)
    {
        for(int j = 0; j < QRS_On.size() - 1; ++j)
        {
            vector <double> x (signals[0].begin() + QRS_On[i][j], signals[0].begin() + QRS_On[i][j + 1]);
            vector <double> y (signals[i+1].begin() + QRS_On[i][j], signals[i+1].begin() + QRS_On[i][j + 1]);

            int iQRS_On = QRS_On[i][j];
            int iQRS_End = QRS_End[i][j];
            int iP_On = P_On [i][j];

			vector <double> xQRSEnd_POn (signals[0].begin() +iQRS_End, signals[0].begin() + iP_On);
			vector <double> yQRSEnd_POn (signals[i+1].begin() + iQRS_End, signals[i+1].begin() + iP_On);


			Filtering(xQRSEnd_POn,yQRSEnd_POn);
			int tPeak = FindTPeak(xQRSEnd_POn,yQRSEnd_POn,iQRS_End,iP_On); 
			int highestVelocity = HighestVelocity(xQRSEnd_POn,yQRSEnd_POn,tPeak, iP_On);

			double a,b,c;
			double vertex = poliFitting(&a,&b,&c,xQRSEnd_POn,yQRSEnd_POn,tPeak,iP_On);

			double tEnd =  FindTEnd_Tangent(xQRSEnd_POn,yQRSEnd_POn,highestVelocity,tPeak);

			double heartAction = x.back() / QRS_On[0].size();

			int BazzetStateTangent, FridericStateTangent, HodgesStateTangent, FraminghamStateTangent;
			int BazzetStateParabol, FridericStateParabol, HodgesStateParabol, FraminghamStateParabol;

			double BazzetValueTangent, FridericValueTangent, HodgesValueTangent, FraminghamValueTangent;
			double BazzetValueParabol, FridericValueParabol, HodgesValueParabol, FraminghamValueParabol;

			double gapQtTangent = iQRS_On - tEnd;
			double gapQtParabol = iQRS_On - vertex;
			
			DispersionEvaluation (gapQtTangent, heartAction, &BazzetStateTangent, &FridericStateTangent, &HodgesStateTangent, &FraminghamStateTangent, &BazzetValueTangent, &FridericValueTangent, &HodgesValueTangent, &FraminghamValueTangent);
			DispersionEvaluation (gapQtParabol, heartAction, &BazzetStateParabol, &FridericStateParabol, &HodgesStateParabol, &FraminghamStateParabol, &BazzetValueParabol, &FridericValueParabol, &HodgesValueParabol, &FraminghamValueParabol);

			tangentEvaluationB[i][j]   = BazzetStateTangent;
			tangentEvaluationFrc[i][j] = FridericStateTangent;
			tangentEvaluationH[i][j]   = HodgesStateTangent;
			tangentEvaluationFhm[i][j] = FraminghamStateTangent;

			parabolEvaluationB[i][j]   = BazzetStateParabol;
			parabolEvaluationFrc[i][j] = FridericStateParabol;
			parabolEvaluationH[i][j]   = HodgesStateParabol;
			parabolEvaluationFhm[i][j] = FraminghamStateParabol;

        }
    }
}
