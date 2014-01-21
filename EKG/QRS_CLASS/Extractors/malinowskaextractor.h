#ifndef MALINOWSKAEXTRACTOR_H
#define MALINOWSKAEXTRACTOR_H

#include "abstractextractor.h"

class MalinowskaExtractor : public AbstractExtractor
{
public:
    MalinowskaExtractor();
    double extractFeature(QList<double> signal);
    QString getName();
    QString getTooltip();
};

#endif // MALINOWSKAEXTRACTOR_H
