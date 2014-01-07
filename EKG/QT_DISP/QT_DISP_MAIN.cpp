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

	int rightBazzetTangent=0;
	int rightFridericTangent=0;
	int rightHodgesTangent=0;
	int rightFraminghamTangent=0;

	int rightBazzetParabol=0;
	int rightFridericParabol=0;
	int rightHodgesParabol=0;
	int rightFraminghamParabol=0;

	int lowBazzetTangent=0;
	int lowFridericTangent=0;
	int lowHodgesTangent=0;
	int lowFraminghamTangent=0;

	int lowBazzetParabol=0;
	int lowFridericParabol=0;
	int lowHodgesParabol=0;
	int lowFraminghamParabol=0;

	int highBazzetTangent=0;
	int highFridericTangent=0;
	int highHodgesTangent=0;
	int highFraminghamTangent=0;

	int highBazzetParabol=0;
	int highFridericParabol=0;
	int highHodgesParabol=0;
	int highFraminghamParabol=0;

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


			switch( BazzetStateTangent )
			{
			case 0:
				rightBazzetTangent++;
				break;
			case 1:
				lowBazzetTangent++;
				break;
			case 2:
				highBazzetTangent++;
				break;
			}

			switch( FridericStateTangent )
			{
			case 0:
				rightFridericTangent++;
				break;
			case 1:
				lowFridericTangent++;
				break;
			case 2:
				highFridericTangent++;
				break;
			}

			switch( HodgesStateTangent )
			{
			case 0:
				rightHodgesTangent++;
				break;
			case 1:
				lowHodgesTangent++;
				break;
			case 2:
				highHodgesTangent++;
				break;
			}

			switch( FraminghamStateTangent )
			{
			case 0:
				rightFraminghamTangent++;
				break;
			case 1:
				lowFraminghamTangent++;
				break;
			case 2:
				highFraminghamTangent++;
				break;
			}

			switch( BazzetStateParabol )
			{
			case 0:
				rightBazzetParabol++;
				break;
			case 1:
				lowBazzetParabol++;
				break;
			case 2:
				highBazzetParabol++;
				break;
			}

			switch( FridericStateParabol )
			{
			case 0:
				rightFridericParabol++;
				break;
			case 1:
				lowFridericParabol++;
				break;
			case 2:
				highFridericParabol++;
				break;
			}

			switch( HodgesStateParabol )
			{
			case 0:
				rightHodgesParabol++;
				break;
			case 1:
				lowHodgesParabol++;
				break;
			case 2:
				highHodgesParabol++;
				break;
			}

			switch( FraminghamStateParabol )
			{
			case 0:
				rightFraminghamParabol++;
				break;
			case 1:
				lowFraminghamParabol++;
				break;
			case 2:
				highFraminghamParabol++;
				break;
			}

        }
					
    }


}
