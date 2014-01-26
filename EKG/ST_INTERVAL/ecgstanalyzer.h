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
        INSUFFICIENT_DATA,
        UNEQUAL_DATA_SIZES
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
                 const QVector<EcgSampleIter> &rData,
                 const QVector<EcgSampleIter> &jData,
                 const QVector<EcgSampleIter> &tEndData,
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
};

//------------------------------------------------------------

#endif // ECGSTANALYZER_H
