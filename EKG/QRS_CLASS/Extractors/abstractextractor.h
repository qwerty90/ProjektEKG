#ifndef ABSTRACTEXTRACTOR_H
#define ABSTRACTEXTRACTOR_H

#include <QList>
#include <QString>

class AbstractExtractor
{
public:
    AbstractExtractor();
    virtual double extractFeature(QList<double> signal) = 0;
    virtual QString getName() = 0;
    virtual QString getTooltip() { return ""; }
};

#endif // ABSTRACTEXTRACTOR_H
