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
    //this->PWaveStart = NULL;//do wywalenia
    this->Waves = new Waves_struct;
    this->Waves->PWaveEnd = NULL;
    this->Waves->QRS_end  = NULL;
    this->Waves->QRS_onset= NULL;
    this->Waves->T_end    = NULL;
    this->Waves->PWaveStart=NULL;
    this->Waves->Count    = 0;
    this->STintervals = NULL;
    this->RR_x = NULL;
    this->RR_y = NULL;
    this->fft_x = NULL;
    this->fft_y = NULL;
    this->classes=NULL;
    this->SigEdr_r=NULL;
    this->SigEdr_q=NULL;
    this->fft_x=NULL;
    this->fft_y=NULL;
    //hrv2
    //twa
    //dfa
    //hrt
    //annotations/settings/info

}

EcgData::EcgData(QString id)
{
    QLOG_INFO() << "Stworzono obiekt typu ecgdata dla "<<id<<"." ;
    this->primary = NULL;
    this->secondary = NULL;
    this->ecg_baselined = NULL;
    this->characteristics=NULL;
    this->Rpeaks = NULL;
    //this->PWaveStart = NULL;//do wywalenia
    this->Waves = new Waves_struct;
    this->Waves->PWaveEnd = NULL;
    this->Waves->QRS_end  = NULL;
    this->Waves->QRS_onset= NULL;
    this->Waves->T_end    = NULL;
    this->Waves->PWaveStart=NULL;
    this->Waves->Count    = 0;
    this->STintervals = NULL;
    this->RR_x = NULL;
    this->RR_y = NULL;
    this->fft_x = NULL;
    this->fft_y = NULL;
    this->classes=NULL;
    this->SigEdr_r=NULL;
    this->SigEdr_q=NULL;
    this->fft_x=NULL;
    this->fft_y=NULL;

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

