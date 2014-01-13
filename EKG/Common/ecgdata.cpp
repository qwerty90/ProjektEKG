#include "ecgdata.h"

EcgData::EcgData(QObject *parent) :
    QObject(parent)
{
    QLOG_INFO() << "Stworzono obiekt typu ecgdata.";
    this->primary = NULL;
    this->secondary = NULL;
    this->ecg_baselined = NULL;
    this->characteristics=NULL;
    this->Rpeaks = NULL;
    this->PWaveStart = NULL;//do wywalenia
    this->classes = NULL;
    this->STintervals = NULL;
    this->RR_x = NULL;
    this->RR_y = NULL;
    this->fft_x = NULL;
    this->fft_y = NULL;
    //hrv2
    //twa
    //dfa
    //hrt
    //annotations/settings/info

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

