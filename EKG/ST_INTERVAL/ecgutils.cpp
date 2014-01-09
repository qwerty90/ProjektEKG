#include "ecgutils.h"

//------------------------------------------------------------

double EcgUtils::sum(const QVector<double> &vector, QVector<double>::const_iterator start, int num)
{
    double sum = 0.0;

    int k = 0;
    for (QVector<double>::const_iterator iter = start; iter != vector.end() && k < num; ++iter, ++k)
        sum += *iter;

    return sum;
}

//------------------------------------------------------------

double EcgUtils::max(const QVector<double> &vector, int *index)
{
    double res = 0.0;
    int idx = -1;
    bool first = true;

    int j = 0;
    QVectorIterator<double> i(vector);
    while (i.hasNext())
    {
        double val = i.next();
        if (first || val > res)
        {

            res = val;
            idx = j;
        }

        first = false;
        j++;
    }

    if (index)
        *index = idx;

    return res;
}

//------------------------------------------------------------

double EcgUtils::min(const QVector<double> &vector, int *index)
{
    double res = 0.0;
    int idx = -1;
    bool first = true;

    int j = 0;
    QVectorIterator<double> i(vector);
    while (i.hasNext())
    {
        double val = i.next();
        if (first || val < res)
        {

            res = val;
            idx = j;
        }

        first = false;
        j++;
    }

    if (index)
        *index = idx;

    return res;
}

//------------------------------------------------------------

QVector<double> EcgUtils::extract(const QVector<double> &src, int start, int end)
{
    return src.mid(start, end - start);
}

//------------------------------------------------------------

QVector<int> EcgUtils::diff(const QVector<QVector<double>::const_iterator> &vector)
{
    if (vector.empty())
        return QVector<int>();

    int num = vector.size() - 1;
    QVector<int> res(num);

    for (int i = 0; i < num; i++)
        res[i] = vector[i + 1] - vector[i];

    return res;
}
