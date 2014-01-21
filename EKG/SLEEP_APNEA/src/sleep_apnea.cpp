#include "sleep_apnea.h"

SleepApnea::SleepApnea(const int samplingFrequency)
    : m_SamplingFrequency(samplingFrequency)
{ }

QVector<BeginEndPair> SleepApnea::processSleepApnea(const QVector<int> &rPeaks)
{
    Q_UNUSED(rPeaks);

    QVector<BeginEndPair> retVal;

    BeginEndPair dummy(1, 3000);
    retVal.append(dummy);

    return retVal;
}


