#ifndef ECGSTANALYZER_H
#define ECGSTANALYZER_H

//------------------------------------------------------------

#include "ecgstdata.h"

//------------------------------------------------------------

enum EcgStPosition {
    ST_POS_NORMAL,
    ST_POS_ELEVATION,
    ST_POS_DEPRESSION
};

enum EcgStShape {
    ST_SHAPE_HORIZONTAL,
    ST_SHAPE_DOWNSLOPING,
    ST_SHAPE_UPSLOPING,
    ST_SHAPE_CONCAVE,
    ST_SHAPE_CONVEX
};

//------------------------------------------------------------

struct EcgStDescriptor
{
    int STOn;
    int STEnd;
    int STMid;

    double offset;
    double slope1;
    double slope2;

    EcgStPosition position;
    EcgStShape shape;
};

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
    void setAlgorithm(const EcgStAlgorithm &value);

    double getBaselineTolerance() const;
    void setBaselineTolerance(double value);

    double getSlopeTolerance() const;
    void setSlopeTolerance(double value);

    QVector<EcgStDescriptor> analyze(const EcgStData &data, double sampleFreq);

    EcgStPosition classifyPosition(double offset);
    EcgStShape classifyShape(double a1, double a2);

    void classifyInterval(EcgStDescriptor &desc);

private:
    unsigned int smoothSize;
    unsigned int detectionSize;
    double morphologyCoeff;

    double baselineTolerance;
    double slopeTolerance;

    EcgStAlgorithm algorithm;
};

//------------------------------------------------------------

#endif // ECGSTANALYZER_H
