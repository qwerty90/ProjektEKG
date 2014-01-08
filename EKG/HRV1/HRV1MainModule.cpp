#include "HRV1MainModule.h"

HRV1MainModule* HRV1MainModule::instance = nullptr;

void HRV1MainModule::createInstance(){
	instance = new HRV1MainModule;
}



HRV1MainModule::~HRV1MainModule(void)
{
	peaks.~vector();
	for(int i = 0; i < dividedPeaks.size(); i++){
		dividedPeaks[i]->~vector();
	}

	dividedPeaks.~vector();
	RRDifferences.~vector();
	toReturnFrequency.xData->~vector();
	toReturnFrequency.yData->~vector();
}

void HRV1MainModule::cutPeaksVector(std::vector<int>* peaks){
	int intervalNumber = 1;
	std::vector<int>* vector;
	std::vector<int>::const_iterator inputIterator = peaks->begin();

	if(peaks->size() == 0){
		return;
	}


	for(int i = 0;; i++){
		vector = new std::vector<int>;
		instance->dividedPeaks.push_back(vector);
		while(*inputIterator <= intervalNumber*instance->samplingFrequency*INTERVAL_LENGTH){
			instance->dividedPeaks.at(i)->push_back(*inputIterator);
			instance->peaks.push_back(*inputIterator);
			inputIterator++;
			if(inputIterator == peaks->end()){
				return;
			}
		}
		intervalNumber++;
	}

	
}


//static function
void HRV1MainModule::prepare(std::vector<int>* peaks, int samplingFrequency){
	if(peaks->size() == 0){
		throw "Empty peaks vector. Aborting.";
	}

	//prepare instance
	if(instance != NULL){
		instance->~HRV1MainModule();
	}
	createInstance();

	//set sampling frequency
	instance->samplingFrequency = samplingFrequency;

	//set RRPeaks
	instance->cutPeaksVector(peaks);

	//evaluate RRdifferences
	instance->evaluateRRDifferences();
}

void HRV1MainModule::evaluateRRDifferences(){
	for(int i = 0; i < peaks.size() - 1; i++){
		RRDifferences.push_back(abs(peaks[i] - peaks[i+1]));
	}
}


//Statistical ANALYSYS FUNCTIONS
//static function
HRV1BundleStatistical HRV1MainModule::evaluateStatistical(){
	if(instance == NULL){
		throw "Module not prepared. Aborting.";
	}

	instance->evaluateRRMeanEntirety();
	instance->evaluateSDNNEntirety();
	instance->evaluateRMSSD();
	instance->evaluateNN50();
	instance->evaluatepNN50();
	instance->evaluateSDANN();
	instance->evaluateSDANNindex();
	instance->evaluateSDSD();
	return instance->toReturnStatistical;
}

double HRV1MainModule::evaluateRRMean(std::vector<int>* peaks){
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

double HRV1MainModule::evaluateSDNN(std::vector<int>* peaks, double mean){
	double sum = 0;
	double difference = 0;

	for(int i = 0; i < peaks->size() - 1; i++){
		difference = abs(peaks->at(i) - peaks->at(i + 1));
		sum = sum + pow(toReturnStatistical.RRMean - difference, 2.0);
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
	std::vector<double> RRMeans;
	double mean;
	for(int i = 0; i < dividedPeaks.size(); i++){
		RRMeans.push_back(evaluateRRMean(dividedPeaks[i]));
	}
	mean = evaluateSimpleMean(&RRMeans);

	toReturnStatistical.SDANN = evaluateStandardDeviation(&RRMeans, mean);
}

void HRV1MainModule::evaluateSDANNindex(){
	std::vector<double> SDNNs;
	std::vector<double> RRMeans;
	for(int i = 0; i < dividedPeaks.size(); i++){
		RRMeans.push_back(evaluateRRMean(dividedPeaks[i]));
		SDNNs.push_back(evaluateSDNN(dividedPeaks[i], RRMeans[i]));
	}

	toReturnStatistical.SDANNindex = evaluateSimpleMean(&SDNNs);
}

void HRV1MainModule::evaluateSDSD(){
	toReturnStatistical.SDSD = evaluateStandardDeviation(&RRDifferences, evaluateSimpleMean(&RRDifferences));
}

double HRV1MainModule::evaluateSimpleMean(std::vector<double>* vector){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += vector->at(i);
	}
	return sum/vector->size();
}

double HRV1MainModule::evaluateSimpleMean(std::vector<int>* vector){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += vector->at(i);
	}
	return sum/vector->size();
}

double HRV1MainModule::evaluateStandardDeviation(std::vector<double>* vector, double mean){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += pow(mean - vector->at(i), 2.0);
	}
	return sqrt(sum/vector->size());
}

double HRV1MainModule::evaluateStandardDeviation(std::vector<int>* vector, double mean){
	double sum = 0;
	for(int i = 0; i < vector->size(); i++){
		sum += pow(mean - vector->at(i), 2.0);
	}
	return sqrt(sum/vector->size());
}

//END OF STATISTICAL ANALYSYS FUNCTIONS

//BEGINNING OF FOURRIER ANALYSYS FUNCTIONS
//static function
HRV1BundleFrequency HRV1MainModule::evaluateFrequency(){
	if(instance == NULL){
		throw "Module not prepared. Aborting.";
	}

	instance->toReturnFrequency.xData = new std::vector<double>;
	instance->toReturnFrequency.yData = new std::vector<double>;

	instance->evaluateSplainInterpolation();
	instance->evaluateFFT();

	return instance->toReturnFrequency;
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
	std::vector<double> tmp;
	alglib::real_1d_array fftTmp;

	for(int i = 1; i < peaks.at(peaks.size() - 1); i = i + (int)(PRECISION*1000))
	{
		tmp.push_back(alglib::spline1dcalc(splineInterpolant, i));
	}

	fftTmp.setlength(tmp.size());

	for(int i = 0; i < tmp.size(); i++){
		fftTmp(i) = tmp.at(i);
	}

	alglib::fftr1d(fftTmp, fftArray);

	toReturnFrequency.xData->push_back(0);
	toReturnFrequency.yData->push_back(fftArray(0).x);
	for(int i = 1; i < fftArray.length(); i++){
		toReturnFrequency.xData->push_back((i)*(1/PRECISION)/(fftArray.length()));
		toReturnFrequency.yData->push_back(fftArray(i).x);
	}
}
