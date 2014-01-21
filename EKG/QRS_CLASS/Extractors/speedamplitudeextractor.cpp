#include "speedamplitudeextractor.h"
#include <cmath>
#include <float.h>
#include <QString>

SpeedAmplitudeExtractor::SpeedAmplitudeExtractor()
{
}

double SpeedAmplitudeExtractor::extractFeature(QList<double> signal)
{
    double maxSpeed = -DBL_MAX;
    double maxSignal = -DBL_MAX;
    double minSignal = DBL_MAX;

    for(int i = 0 ; i < signal.count(); i++)
    {
        if (i > 2)
        {
            double speed = signal.at(i) + signal.at(i-2) - 2*signal.at(i-2);
            if (speed > maxSpeed)
                maxSpeed = speed;
        }

        if (signal.at(i) > maxSignal)
            maxSignal = signal.at(i);

        if (signal.at(i) < minSignal)
            minSignal = signal.at(i);
    }

    double maxAmp = fabs(maxSignal - minSignal);

    return 1000*(maxSpeed/maxAmp);
}

QString SpeedAmplitudeExtractor::getName()
{
    return "Speed/Amplitude :";
}

QString SpeedAmplitudeExtractor::getTooltip()
{
    return "Ratio of max speed in QRS sample to amplitude of QRS Sample";
}
