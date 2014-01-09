#ifndef ECGUTILS_H
#define ECGUTILS_H

//------------------------------------------------------------

#include <QVector>

//------------------------------------------------------------

namespace EcgUtils
{
double sum(const QVector<double> &vector, QVector<double>::const_iterator start, int num);
double max(const QVector<double> &vector, int *index = 0);
double min(const QVector<double> &vector, int *index = 0);
QVector<double> extract(const QVector<double> &src, int start, int end);
QVector<int> diff(const QVector<QVector<double>::const_iterator> &vector);
}

//------------------------------------------------------------

#endif // ECGUTILS_H
