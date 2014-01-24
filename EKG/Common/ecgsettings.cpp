#include "ecgsettings.h"

EcgSettings::EcgSettings(QObject *parent) :
    QObject(parent)
{
    //ecg baseline init
    this->EcgBaselineMode = 0;    
    this->averaging_time=0;
    this->avgWindowSize =0;
    this->coeff_set     =0;
    //Rpeaks init
    this->RPeaksMode = 1;
    //waves init
    this->P_on_checked   =true;
    this->P_end_checked  =true;
    this->Qrs_on_checked =true;
    this->Qrs_end_checked=true;
    //st interval init
    this->detect_window = 30;
    this->smooth_window = 4;
    this->morph_coeff   = 6.0;
    this->level_tresh   = 0.15;
    this->slope_tresh   = 35;
    //SigEdr
    this->SigEdr_qrs = true;
    this->SigEdr_rpeaks=true;
}
