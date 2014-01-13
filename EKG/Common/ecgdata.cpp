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
        QLOG_INFO() << "Primary signal will be processed.";
        return this->primary;
    }
    else
    {
        QLOG_INFO() << "Secondary signal will be processed.";
        return this->secondary;
    }
}

