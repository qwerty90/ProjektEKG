#ifndef ECGDATA_H
#define ECGDATA_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QPointF>
#include <QMap>
#include "ecgannotation.h"
#include "ecginfo.h"
#include "ecgsettings.h"
#include "QsLog/QsLog.h"
#include "Waves/src/waves.h"
#include "QRS_CLASS/qrsclass.h"
#include "SLEEP_APNEA/src/sleep_apnea.h"
#include "ECG_BASELINE/src/butter.h"
//#include "QT_DISP/Evaluation.h"
#include "QT_DISP/QT_DISP.h"

#include "../ST_INTERVAL/ecgstdescriptor.h"

struct Waves_struct
{
    QVector<QVector<double>::const_iterator> * PWaveStart;
    QVector<QVector<double>::const_iterator> * PWaveEnd;
    QVector<QVector<double>::const_iterator> * QRS_onset;
    QVector<QVector<double>::const_iterator> * QRS_end;
    QVector<QVector<double>::const_iterator> * T_end;

    int Count;
};
struct VCG_input
{
    QVector<double> *I  ;
    QVector<double> *II ;
    QVector<double> *V1 ;
    QVector<double> *V2 ;
    QVector<double> *V3 ;
    QVector<double> *V4 ;
    QVector<double> *V5 ;
    QVector<double> *V6 ;
};

class EcgData : public QObject
{
    Q_OBJECT        
public:

    QString RecordId;

    // czasy
    //QVector<double> *times;
    //wartosci liczbowe z dwoch elektrod
    QVector<double> *primary;
    QVector<double> *secondary;

    //przefiltrowany sygnal ekg - wyjscie modulu ECG_BASELINE
    QVector<double> *ecg_baselined;
    QVector<QPointF> *characteristics;
    QVector<ButterCoefficients> *butter_coeffs;

    //numery probek zalamkow R - wyjscie modulu R_PEAKS
    QVector<QVector<double>::const_iterator> *Rpeaks;
    QVector<unsigned int> Rpeaks_uint;
    //QVector<int>          Rpeaks_int ;//po raz trzeci - wygrałem!!!

    //punkty charakterystyczne - wyjscie modulu WAVES
    Waves_struct *Waves;
    //QVector<QVector<double>::const_iterator> *PWaveStart;

    //Wykryte klasy zespolu QRS - wyjscie modulu QRS_CLASS
    QVector<QRSClass>* classes;

    // modul ST_INTERVAL
    QList<EcgStDescriptor> *STintervals;

    // HRV1
    //dane statystyczne
    double Mean, SDNN, RMSSD, RR50, RR50Ratio, SDANN, SDANNindex, SDSD;
    QVector<double> *RR_x;
    QVector<double> *RR_y;

    //HRV1dane czestotliwosciowe
    double TP, HF, LF, VLF, ULF, LFHF;
    QVector<double> *fft_x;
    QVector<double> *fft_y;

    //modul HRT
    double turbulence_slope, turbulence_onset;
    double hrt_a , hrt_b;
    int vpbs_detected_count;
    QVector<double> *hrt_tachogram;

    //modul ATRIAL_FIBR
    double PWaveOccurenceRatio;
    double RRIntEntropy;
    double RRIntDivergence;
    bool   AtrialFibr;

    //modul SigEdr
    QVector<double> *SigEdr_r;
    QVector<double> *SigEdr_q;

    //modul sleep apnea
    QVector<BeginEndPair>    *SleepApnea;
    QVector<double>          *SleepApnea_plot;
    QVector<QVector<double>> *SleepApnea_wykresy;

    //modul QtDisp (bez TWaves)
    QVector<Evaluation> *evaluations;

    //modul VCG_LOOP
    VCG_input *VCG_raw;

    QList<EcgAnnotation> *annotations;
    EcgInfo *info;

    EcgSettings *settings;

    EcgData(QString id);

    explicit EcgData(QObject *parent = 0);

    QVector<double> *GetCurrentSignal();
    
signals:
    
public slots:
    
};

#endif // ECGDATA_H
