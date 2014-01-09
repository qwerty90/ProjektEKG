#ifndef ECGSTANALYZER_H
#define ECGSTANALYZER_H

//------------------------------------------------------------

#include "ecgstdescriptor.h"

//------------------------------------------------------------

enum EcgStAlgorithm {
    ST_LINEAR,
    ST_QUADRATIC
};

//------------------------------------------------------------

class EcgStAnalyzer
{
public:
    EcgStAnalyzer();

    unsigned int getSmoothSize() const;
    void setSmoothSize(unsigned int value);

    unsigned int getDetectionSize() const;
    void setDetectionSize(unsigned int value);

    double getMorphologyCoeff() const;
    void setMorphologyCoeff(double value);

    EcgStAlgorithm getAlgorithm() const;
    void setAlgorithm(EcgStAlgorithm value);

    double getLevelThreshold() const;
    void setLevelThreshold(double value);

    double getSlopeThreshold() const;
    void setSlopeThreshold(double value);

    QList<EcgStDescriptor> analyze(const QVector<double> &ecgSamples,
                                   const QVector<QVector<double>::const_iterator> &rData,
                                   const QVector<QVector<double>::const_iterator> &jData,
                                   const QVector<QVector<double>::const_iterator> &tEndData,
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

    EcgStAlgorithm algorithm;
};

//------------------------------------------------------------

#endif // ECGSTANALYZER_H
