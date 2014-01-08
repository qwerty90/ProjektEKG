#ifndef ECGUTILS_H
#define ECGUTILS_H

//------------------------------------------------------------

#include <QVector>

//------------------------------------------------------------

namespace EcgUtils
{
double sum(const QVector<double> &vector);
double max(const QVector<double> &vector, int *index = 0);
double min(const QVector<double> &vector, int *index = 0);
QVector<double> extract(const QVector<double> &src, int start, int end);
QVector<int> diff(const QVector<int> &vector);
}

//------------------------------------------------------------

#endif // ECGUTILS_H
