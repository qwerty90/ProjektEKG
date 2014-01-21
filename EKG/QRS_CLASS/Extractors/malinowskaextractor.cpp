#include "malinowskaextractor.h"
#include <cmath>
#include <QString>

MalinowskaExtractor::MalinowskaExtractor()
{
}

double MalinowskaExtractor::extractFeature(QList<double> signal)
{
    double sum = 0;
    double area = 0;
    for(int i = 0; i < signal.count(); i++)
    {
        if (i > 1)
            area += fabs(signal.at(i) - signal.at(i-1));

        sum += fabs(signal.at(i));
    }

    return 10*sum/area;
}

QString MalinowskaExtractor::getName()
{
    return "Malinowska:";
}

QString MalinowskaExtractor::getTooltip()
{
    return "Ratio of perimeter to area of QRS segment";
}

