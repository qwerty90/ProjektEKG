#include "ecgstanalyzer.h"
#include "ecgutils.h"

#include <cmath>
#include <algorithm>

//------------------------------------------------------------

#define RAD_TO_DEG(x)  ((x) * 180 / 3.14159265359)

//------------------------------------------------------------

namespace EcgStDefaults
{
const int SMOOTH_SIZE           = 4;
const int DETECTION_SIZE        = 30;
const double MORPHOLOGY_COEFF   = 6.0;
const EcgStAlgorithm ALGORITHM  = ST_LINEAR;

const double BASELINE_TOLERANCE = 0.15;
const double SLOPE_TOLERANCE    = 35.0;
}

//------------------------------------------------------------

EcgStAnalyzer::EcgStAnalyzer() :
    smoothSize(EcgStDefaults::SMOOTH_SIZE),
    detectionSize(EcgStDefaults::DETECTION_SIZE),
    morphologyCoeff(EcgStDefaults::MORPHOLOGY_COEFF),
    baselineTolerance(EcgStDefaults::BASELINE_TOLERANCE),
    slopeTolerance(EcgStDefaults::SLOPE_TOLERANCE),
    algorithm(EcgStDefaults::ALGORITHM)
{
}

//------------------------------------------------------------

unsigned int EcgStAnalyzer::getSmoothSize() const
{
    return smoothSize;
}

//------------------------------------------------------------

void EcgStAnalyzer::setSmoothSize(unsigned int value)
{
    smoothSize = value;
}

//------------------------------------------------------------

unsigned int EcgStAnalyzer::getDetectionSize() const
{
    return detectionSize;
}

//------------------------------------------------------------

void EcgStAnalyzer::setDetectionSize(unsigned int value)
{
    detectionSize = value;
}

//------------------------------------------------------------

double EcgStAnalyzer::getMorphologyCoeff() const
{
    return morphologyCoeff;
}

//------------------------------------------------------------

void EcgStAnalyzer::setMorphologyCoeff(double value)
{
    if (value < 0.0)
        value = 0.0;

    morphologyCoeff = value;
}

//------------------------------------------------------------

EcgStAlgorithm EcgStAnalyzer::getAlgorithm() const
{
    return algorithm;
}

//------------------------------------------------------------

void EcgStAnalyzer::setAlgorithm(EcgStAlgorithm value)
{
    algorithm = value;
}

//------------------------------------------------------------

double EcgStAnalyzer::getBaselineTolerance() const
{
    return baselineTolerance;
}

//------------------------------------------------------------

void EcgStAnalyzer::setBaselineTolerance(double value)
{
    baselineTolerance = value;
}

//------------------------------------------------------------

double EcgStAnalyzer::getSlopeTolerance() const
{
    return slopeTolerance;
}

//------------------------------------------------------------

void EcgStAnalyzer::setSlopeTolerance(double value)
{
    slopeTolerance = value;
}

//------------------------------------------------------------

QList<EcgStDescriptor> EcgStAnalyzer::analyze(const QVector<double> &ecgSamples,
                                              const QVector<QVector<double>::const_iterator> &rData,
                                              const QVector<QVector<double>::const_iterator> &jData,
                                              const QVector<QVector<double>::const_iterator> &tEndData,
                                              double sampleFreq)
{
    QList<EcgStDescriptor> result;

    int num = rData.size();
    int snum = ecgSamples.size();

    if (num < 2 || snum == 0)
        return result;

    int p = smoothSize;
    int w = detectionSize;
    double lamdba = morphologyCoeff;

    QVector<QVector<double>::const_iterator> stOn(num);
    QVector<QVector<double>::const_iterator> stEnd(num);

    int i;

    // detect STend points
    for (i = 0; i < num; i++)
    {
        QVector<double>::const_iterator ka = jData[i];
        QVector<double>::const_iterator kb = tEndData[i];

        QVector<double> aVal(kb - ka + 1);
        QVector<double>::iterator aIter = aVal.begin();

        for (QVector<double>::const_iterator k = ka; k <= kb; ++k)
        {
            QVector<double>::const_iterator ke = std::max(ecgSamples.constBegin(), std::min(k + p, ecgSamples.end() - 1));
            int nsr = ke - k + p + 1;
//            QVector<double> wnd = data.ecgSamples.mid(k - p - 1, nsr);
            double sk = EcgUtils::sum(ecgSamples, k - (p + 1), nsr) / nsr;
            ke = std::max(ecgSamples.constBegin(), std::min(k + w - 1, ecgSamples.constEnd() - 1));

            QVector<double> sqr(ke - k);
            for (int j = 0; j < sqr.size(); j++) {
                double smp = *(k + (j - 1)) - sk;

                if (algorithm == ST_QUADRATIC)
                    sqr[j] = smp * smp;
                else
                    sqr[j] = smp;
            }

            *aIter = EcgUtils::sum(sqr, sqr.begin(), sqr.size());
            ++aIter;
        }

        int kp, kp1, kp2;
        double ap1 = EcgUtils::max(aVal, &kp1);
        double ap2 = EcgUtils::min(aVal, &kp2);

        double at = fabs(ap1) / fabs(ap2);
        if ((1.0 / lamdba < at) && (at < lamdba))
            kp = std::min(kp1, kp2);
        else
            kp = fabs(ap1) > fabs(ap2) ? kp1 : kp2;

        stEnd[i] = ka + kp;
    }

    // calculate heart rate
    QVector<int> rr = EcgUtils::diff(rData);
    QVector<double> hr(num);
    for (i = 0; i < num - 1; i++)
        hr[i] = 60.0 / ((double) rr[i] / sampleFreq);
    hr[num - 1] = hr[num - 2];

    for (i = 0; i < num; i++)
    {
        double rt = stEnd[i] - rData[i];

        double x;
        double hrc = hr[i];
        if (hrc < 100)
            x = 0.4;
        else if (hrc < 110)
            x = 0.45;
        else if (hrc < 120)
            x = 0.5;
        else
            x = 0.55;

        QVector<double>::const_iterator test = rData[i] + ((int) round(x * rt));
        stOn[i] = std::min(jData[i] + 1, test);
    }

    // create and classify interval descriptors
    for (i = 0; i < num; i++)
    {
        EcgStDescriptor desc;

        desc.STOn = stOn[i];
        desc.STEnd = stEnd[i];
        desc.STMid = desc.STOn + (int) round((desc.STEnd - desc.STOn) / 2.0);

        desc.offset = *desc.STMid;

        QVector<double>::const_iterator x1 = desc.STOn;
        QVector<double>::const_iterator x2 = desc.STMid;
        double y1 = *x1;
        double y2 = *x2;
        double d1 = (y1 - y2) / ((x1 - x2) / sampleFreq);
        desc.slope1 = RAD_TO_DEG(atan(d1));

        x1 = desc.STMid;
        x2 = desc.STEnd;
        y1 = *x1;
        y2 = *x2;
        double d2 = (y1 - y2) / ((x1 - x2) / sampleFreq);
        desc.slope2 = RAD_TO_DEG(atan(d2));

        classifyInterval(desc);

        result.push_back(desc);
    }

    return result;
}

//------------------------------------------------------------

EcgStPosition EcgStAnalyzer::classifyPosition(double offset)
{
    if (fabs(offset) <= baselineTolerance)
        return ST_POS_NORMAL;
    else if (offset > baselineTolerance)
        return ST_POS_ELEVATION;
    else
        return ST_POS_DEPRESSION;
}

//------------------------------------------------------------

EcgStShape EcgStAnalyzer::classifyShape(double a1, double a2)
{
    if (fabs(a1) <= slopeTolerance || fabs(a2) <= slopeTolerance)
    {
        if (a1 > slopeTolerance || a2 > slopeTolerance)
            return ST_SHAPE_UPSLOPING;
        else if (a1 < -slopeTolerance || a2 < -slopeTolerance)
            return ST_SHAPE_DOWNSLOPING;
        else
            return ST_SHAPE_HORIZONTAL;
    }
    else if (a1 > slopeTolerance && a2 > slopeTolerance)
        return ST_SHAPE_UPSLOPING;
    else if (a1 < -slopeTolerance && a2 < -slopeTolerance)
        return ST_SHAPE_DOWNSLOPING;
    else if (a1 < -slopeTolerance && a2 > slopeTolerance)
        return ST_SHAPE_CONCAVE;
    else /*if (a1 > slopeTolerance && a2 < -slopeTolerance)*/
        return ST_SHAPE_CONVEX;
}

//------------------------------------------------------------

void EcgStAnalyzer::classifyInterval(EcgStDescriptor &desc)
{
    desc.position = classifyPosition(desc.offset);
    desc.shape = classifyShape(desc.slope1, desc.slope2);
}

