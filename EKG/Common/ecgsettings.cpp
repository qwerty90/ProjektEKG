#include "ecgsettings.h"

EcgSettings::EcgSettings(QObject *parent) :
    QObject(parent)
{
    this->EcgBaselineMode = 0;
    this->RPeaksMode = 1;

    this->qrsClassSettings.clusterer = GMeansClusterer;
    this->qrsClassSettings.insideIterations = 1000;
    this->qrsClassSettings.maxClusterNo = 10;
    this->qrsClassSettings.minClusterNo = 1;
    this->qrsClassSettings.maxIterations = 5;
    this->qrsClassSettings.parallelExecution = true;
}
