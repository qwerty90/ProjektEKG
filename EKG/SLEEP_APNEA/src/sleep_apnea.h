#ifndef SLEEP_APNEA_H
#define SLEEP_APNEA_H

#include "QVector"
#include "QPair"

typedef QPair<unsigned long int, unsigned long int> BeginEndPair;

class SleepApnea
{
public:
    SleepApnea(const int samplingFrequency);

    QVector<BeginEndPair> processSleepApnea(const QVector<int> &rPeaks);


private:
    int m_SamplingFrequency;
};

#endif // SLEEP_APNEA_H
