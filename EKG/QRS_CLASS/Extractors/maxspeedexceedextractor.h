#ifndef MAXSPEEDEXCEEDEXTRACTOR_H
#define MAXSPEEDEXCEEDEXTRACTOR_H

#include "abstractextractor.h"

class MaxSpeedExceedExtractor : public AbstractExtractor
{
public:
    MaxSpeedExceedExtractor();
    double extractFeature(QList<double> signal);
    QString getName();
    QString getTooltip();
};

#endif // MAXSPEEDEXCEEDEXTRACTOR_H
