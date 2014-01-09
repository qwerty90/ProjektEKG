#ifndef ECGSTDATA_H
#define ECGSTDATA_H

//------------------------------------------------------------

#include <QVector>

//------------------------------------------------------------

struct EcgStData
{
    QVector<double> ecgSamples;
    QVector<QVector<double>::const_iterator> rData;
    QVector<QVector<double>::const_iterator> jData;
    QVector<QVector<double>::const_iterator> tEndData;
};

//------------------------------------------------------------

#endif // ECGSTDATA_H
