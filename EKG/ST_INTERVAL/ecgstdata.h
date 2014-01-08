#ifndef ECGSTDATA_H
#define ECGSTDATA_H

//------------------------------------------------------------

#include <QVector>

//------------------------------------------------------------

struct EcgStData
{
    QVector<double> ecgSamples;
    QVector<int> rData;
    QVector<int> jData;
    QVector<int> tEndData;
};

//------------------------------------------------------------

#endif // ECGSTDATA_H
