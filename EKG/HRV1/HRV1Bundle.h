//output structures

struct HRV1BundleStatistical
{
	//statical analysys parameters
	double RRMean;
	double SDNN;
	double RMSSD;
	double NN50;
	double pNN50;
	double SDANN;
	double SDANNindex;
	double SDSD;

};

struct HRV1BundleFrequency
{
	//fourrier graph	
    QVector<double>* xData;
    QVector<double>* yData;

    //Spline interpolant (RR function)
    QVector<double>* rrXData;
    QVector<double>* rrYData;

	//frequency analysys parameters 
	//NOT YET WORKING
	double TP;
	double HF;
	double LF;
	double VLF;
	double ULF;
	double LFHF;
};
