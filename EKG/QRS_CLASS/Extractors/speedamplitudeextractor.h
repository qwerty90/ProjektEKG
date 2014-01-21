#ifndef SPEEDAMPLITUDEEXTRACTOR_H
#define SPEEDAMPLITUDEEXTRACTOR_H

#include "abstractextractor.h"

class SpeedAmplitudeExtractor : public AbstractExtractor
{
public:
    SpeedAmplitudeExtractor();
    double extractFeature(QList<double> signal);
    QString getName();
    QString getTooltip();
};

#endif // SPEEDAMPLITUDEEXTRACTOR_H
