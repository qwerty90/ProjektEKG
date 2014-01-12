#include "ecgdata.h"

EcgData::EcgData(QObject *parent) :
    QObject(parent)
{
}

EcgData::EcgData(QString id)
{
    this->RecordId = id;
    this->settings = new EcgSettings();
}

QVector<double>* EcgData::GetCurrentSignal()
{
    if(this->settings->signalIndex == 0)
    {
        return this->primary;
    }
    else
    {
        return this->secondary;
    }
}

