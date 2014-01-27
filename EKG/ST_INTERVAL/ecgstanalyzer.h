#ifndef ECGSTANALYZER_H
#define ECGSTANALYZER_H

//------------------------------------------------------------

#include "ecgstdescriptor.h"

//------------------------------------------------------------

typedef QVector<double>::const_iterator EcgSampleIter;

//------------------------------------------------------------

class EcgStAnalyzer
{
public:
    enum AlgorithmType
    {
        LINEAR,
        QUADRATIC
    };

    enum ErrorType
    {
        NO_ERROR,
        NO_SAMPLES_PROVIDED,
        INSUFFICIENT_DATA
    };

    EcgStAnalyzer();

    unsigned int getSmoothSize() const;
    void setSmoothSize(unsigned int value);

    unsigned int getDetectionSize() const;
    void setDetectionSize(unsigned int value);

    double getMorphologyCoeff() const;
    void setMorphologyCoeff(double value);

    AlgorithmType getAlgorithm() const;
    void setAlgorithm(AlgorithmType value);

    double getLevelThreshold() const;
    void setLevelThreshold(double value);

    double getSlopeThreshold() const;
    void setSlopeThreshold(double value);

    QList<EcgStDescriptor> getResult() const;
    ErrorType getLastError() const;

    bool analyze(const QVector<double> &ecgSamples,
                 const QVector<EcgSampleIter> &rVec,
                 const QVector<EcgSampleIter> &jVec,
                 const QVector<EcgSampleIter> &tEndVec,
                 double sampleFreq);

    EcgStPosition classifyPosition(double offset);
    EcgStShape classifyShape(double a1, double a2);

    void classifyInterval(EcgStDescriptor &desc);

private:
    unsigned int smoothSize;
    unsigned int detectionSize;
    double morphologyCoeff;

    double levelThreshold;
    double slopeThreshold;

    AlgorithmType algorithm;

    QList<EcgStDescriptor> result;
    ErrorType lastError;

    QVector<EcgSampleIter> rData;
    QVector<EcgSampleIter> jData;
    QVector<EcgSampleIter> tEndData;

    void clearData();
    void prepareData(const QVector<double> &ecgSamples,
                     const QVector<EcgSampleIter> &rVec,
                     const QVector<EcgSampleIter> &jVec,
                     const QVector<EcgSampleIter> &tEndVec);
};

//------------------------------------------------------------

#endif // ECGSTANALYZER_H
