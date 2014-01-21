#include "maxspeedexceedextractor.h"
#include <QList>
#include <cmath>
#include <QDebug>

MaxSpeedExceedExtractor::MaxSpeedExceedExtractor()
{
}

double MaxSpeedExceedExtractor::extractFeature(QList<double> signal)
{
    double maxSpeed = 0;
    int signalSize = signal.count();

    QList<double> speedList;
    for(int i = 1 ; i < signalSize; i++)
    {
        double speed = fabs(signal.at(i) - signal.at(i-1));

        if (speed > maxSpeed)
            maxSpeed = speed;

        speedList.append(speed);
    }

    maxSpeed *= 0.4;

    int samplesCount = 0;
    foreach(double currSpeed, speedList)
        if (currSpeed > maxSpeed)
            samplesCount++;

    double toReturn = double(samplesCount)/double(signalSize);
    return toReturn;
}

QString MaxSpeedExceedExtractor::getName()
{
    return "SpeedExceededSamples";
}

QString MaxSpeedExceedExtractor::getTooltip()
{
    return "Ratio of number of samples that exceeded 40% of maximum sample speed to number of samples";
}
