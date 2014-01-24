#include "HRV1MainModule.h"

HRV1MainModule::HRV1MainModule(void){
}

HRV1MainModule::~HRV1MainModule(void)
{
	for(int i = 0; i < dividedPeaks.size(); i++){
        delete dividedPeaks[i];
	}

    //delete toReturnFrequency.xData;
    //delete toReturnFrequency.yData;
    //delete toReturnFrequency.rrXData;
    //delete toReturnFrequency.rrYData;
}


void HRV1MainModule::cutPeaksVector(QVector<int>* peaks){
	int intervalNumber = 1;
    QVector<double>* vector;
    QVector<int>::const_iterator inputIterator = peaks->begin();

	if(peaks->size() == 0){
		return;
	}


	for(int i = 0;; i++){
        vector = new QVector<double>;
        dividedPeaks.push_back(vector);
        while(*inputIterator <= intervalNumber*samplingFrequency*INTERVAL_LENGTH){
            dividedPeaks.at(i)->push_back(*inputIterator*1000.0/samplingFrequency);
            this->peaks.push_back(*inputIterator*1000.0/samplingFrequency);
			inputIterator++;
			if(inputIterator == peaks->end()){
				return;
			}
		}
		intervalNumber++;
	}
}

void HRV1MainModule::prepare(QVector<int>* peaks, int samplingFrequency){
	if(peaks->size() == 0){
		throw "Empty peaks vector. Aborting.";
	}

	//set sampling frequency
    this->samplingFrequency = samplingFrequency;

	//set RRPeaks
    cutPeaksVector(peaks);

	//evaluate RRdifferences
    evaluateRRDifferences();
}

void HRV1MainModule::evaluateRRDifferences(){
	for(int i = 0; i < peaks.size() - 1; i++){
		RRDifferences.push_back(abs(peaks[i] - peaks[i+1]));
	}
}


//Statistical ANALYSYS FUNCTIONS
HRV1BundleStatistical HRV1MainModule::evaluateStatistical(){

    evaluateRRMeanEntirety();
    evaluateSDNNEntirety();
    evaluateRMSSD();
    evaluateNN50();
    evaluatepNN50();
    evaluateSDANN();
    evaluateSDANNindex();
    evaluateSDSD();
    return this->toReturnStatistical;
}

double HRV1MainModule::evaluateRRMean(QVector<double>* peaks){
	double sum=0;
	for(int i=0;i<peaks->size()-1;i++){
		 sum =sum + abs(peaks->at(i)-peaks->at(i+1));
	}
    return sum/(peaks->size() - 1);
}

void HRV1MainModule::evaluateRRMeanEntirety(){
	toReturnStatistical.RRMean = evaluateRRMean(&peaks);
}

void HRV1MainModule::evaluateSDNNEntirety(){
	toReturnStatistical.SDNN = evaluateSDNN(&peaks, toReturnStatistical.RRMean);
}

double HRV1MainModule::evaluateSDNN(QVector<double>* peaks, double mean){
	double sum = 0;
	double difference = 0;

	for(int i = 0; i < peaks->size() - 1; i++){
		difference = abs(peaks->at(i) - peaks->at(i + 1));
        sum = sum + pow(mean - difference, 2.0);
	}

	return sqrt(sum / (peaks->size() - 1));
}

void HRV1MainModule::evaluateRMSSD(){
	double sum = 0;
	double difference1 = 0;
	double difference2 = 0;

	for(int i = 0; i < peaks.size() - 2; i++){
		difference1 = abs(peaks.at(i) - peaks.at(i + 1));
		difference2 = abs(peaks.at(i + 1) - peaks.at(i + 2));
		sum = sum + pow(difference1 - difference2, 2.0);
	}

	toReturnStatistical.RMSSD = sqrt(sum / (peaks.size() - 2));
}

void HRV1MainModule::evaluateNN50(){
	double sum = 0;
	double difference1;
	double difference2;

	for(int i = 0; i < peaks.size() - 2; i++){
		difference1 = abs(peaks.at(i) - peaks.at(i + 1));
		difference2 = abs(peaks.at(i + 1) - peaks.at(i + 2));
		if(abs(difference1 - difference2) > 50.0){
			sum++;
		}
	}

	toReturnStatistical.NN50 = sum;
}

void HRV1MainModule::evaluatepNN50(){
	toReturnStatistical.pNN50 = ((toReturnStatistical.NN50)/(peaks.size() - 2))*100;
}

void HRV1MainModule::evaluateSDANN(){
    QVector<double> RRMeans;
	double mean;
	for(int i = 0; i < dividedPeaks.size(); i++){
		RRMeans.push_back(evaluateRRMean(dividedPeaks[i]));
	}
	mean = evaluateSimpleMean(&RRMeans);

	toReturnStatistical.SDANN = evaluateStandardDeviation(&RRMeans, mean);
}

void HRV1MainModule::evaluateSDANNindex(){
    QVector<double> SDNNs;
    QVector<double> RRMeans;
	for(int i = 0; i < dividedPeaks.size(); i++){
		RRMeans.push_back(evaluateRRMean(dividedPeaks[i]));
		SDNNs.push_back(evaluateSDNN(dividedPeaks[i], RRMeans[i]));
	}

	toReturnStatistical.SDANNindex = evaluateSimpleMean(&SDNNs);
}

void HRV1MainModule::evaluateSDSD(){
    QVector<double> RRtoRRDifferences;
    for(int i = 0; i < RRDifferences.size() - 1; i++){
        RRtoRRDifferences.push_back(abs(RRDifferences.at(i) - RRDifferences.at(i+1)));
    }
    toReturnStatistical.SDSD = evaluateStandardDeviation(&RRtoRRDifferences, evaluateSimpleMean(&RRtoRRDifferences));
}

double HRV1MainModule::evaluateSimpleMean(QVector<double>* vector){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += vector->at(i);
	}
	return sum/vector->size();
}

double HRV1MainModule::evaluateStandardDeviation(QVector<double>* vector, double mean){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += pow(mean - vector->at(i), 2.0);
	}
	return sqrt(sum/vector->size());
}

//END OF STATISTICAL ANALYSYS FUNCTIONS

//BEGINNING OF FOURRIER ANALYSYS FUNCTIONS
HRV1BundleFrequency HRV1MainModule::evaluateFrequency(){

    toReturnFrequency.xData = new QVector<double>;
    toReturnFrequency.yData = new QVector<double>;

    toReturnFrequency.rrXData = new QVector<double>;
    toReturnFrequency.rrYData = new QVector<double>;

    evaluateSplainInterpolation();
    evaluateFFT();

    evaluateTP();
    evaluateHF();
    evaluateLF();
    evaluateVLF();
    evaluateULF();
    evaluateLFHF();

    return  toReturnFrequency;
}

void HRV1MainModule::evaluateSplainInterpolation(){
	alglib::real_1d_array x, y;

	x.setlength(peaks.size() - 1);
	y.setlength(peaks.size() - 1);

	for(int i = 0; i < peaks.size() - 1; i++){
		x(i) = (peaks.at(i) + peaks.at(i+1)) / 2;
		y(i) = RRDifferences.at(i);
	}

	alglib::spline1dbuildcubic(x, y, splineInterpolant);
}

void HRV1MainModule::evaluateFFT(){
    alglib::real_1d_array fftTmp;
    QVector<double> fftModules;
    QVector<double> fftFrequences;
    double maxModuleValue;

	for(int i = 1; i < peaks.at(peaks.size() - 1); i = i + (int)(PRECISION*1000))
	{
        toReturnFrequency.rrYData->push_back(alglib::spline1dcalc(splineInterpolant, i));
        toReturnFrequency.rrXData->push_back(i);
	}



    fftTmp.setlength(toReturnFrequency.rrYData->size());

    for(int i = 0; i < toReturnFrequency.rrYData->size(); i++){
        fftTmp(i) = toReturnFrequency.rrYData->at(i);
	}

	alglib::fftr1d(fftTmp, fftArray);

    //fftFrequences.push_back(0);
    //fftModules.push_back(sqrt(pow(fftArray(0).x, 2) + pow(fftArray(0).y, 2))*2/fftArray.length());
	for(int i = 1; i < fftArray.length(); i++){
        fftFrequences.push_back((i)*(1/PRECISION)/(fftArray.length()));
        fftModules.push_back(sqrt(pow(fftArray(i).x, 2) + pow(fftArray(i).y, 2))*2/fftArray.length());
	}

    for(int i = 0; fftFrequences.at(i) <= FREQUENCY_TRESHOLD; i++)
    {
        if(fftModules.at(i) > maxModuleValue)
        {
            maxModuleValue = fftModules.at(i);
        }
    }

    for(int i = 0; fftFrequences.at(i) <= FREQUENCY_TRESHOLD; i++)
    {
        toReturnFrequency.xData->push_back(fftFrequences.at(i));
        toReturnFrequency.yData->push_back(pow(fftModules.at(i),2));
    }
}

double HRV1MainModule::evaluateFrequencyPower(double low, double high){
    //double stepSize = toReturnFrequency.xData->at(1) - toReturnFrequency.xData->at(0);
    double stepSize = 1.0;
    double toReturn = 0;

    for(int i = 0; i < toReturnFrequency.yData->size(); i++){
        if((toReturnFrequency.xData->at(i) >= low) &&(toReturnFrequency.xData->at(i) <= high)){
            toReturn = toReturn + toReturnFrequency.yData->at(i);
        }
    }

    toReturn = toReturn * stepSize;

    return toReturn;
}

void HRV1MainModule::evaluateTP(){
    toReturnFrequency.TP = evaluateFrequencyPower(0.0, FREQUENCY_TRESHOLD);
}

void HRV1MainModule::evaluateHF(){
    toReturnFrequency.HF = evaluateFrequencyPower(HF_RANGE, FREQUENCY_TRESHOLD);
}

void HRV1MainModule::evaluateLF(){
    toReturnFrequency.LF = evaluateFrequencyPower(LF_RANGE, HF_RANGE);
}

void HRV1MainModule::evaluateVLF(){
    toReturnFrequency.VLF = evaluateFrequencyPower(VLF_RANGE, LF_RANGE);
}

void HRV1MainModule::evaluateULF(){
    toReturnFrequency.ULF = evaluateFrequencyPower(0.0, VLF_RANGE);
}

void HRV1MainModule::evaluateLFHF(){
    toReturnFrequency.LFHF = toReturnFrequency.LF/toReturnFrequency.HF;
}


