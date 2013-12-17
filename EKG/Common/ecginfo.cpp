#include "ecginfo.h"

EcgInfo::EcgInfo(QObject *parent) :
    QObject(parent)
{
}

void EcgInfo::parseFrequency()
{
    int n = this->samplingFreq.length();
    QString value = this->samplingFreq.mid(0, n-2);
    this->frequencyValue = value.toFloat();
}
