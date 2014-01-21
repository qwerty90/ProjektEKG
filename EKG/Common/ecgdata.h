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

class EcgData : public QObject
{
    Q_OBJECT        
public:

    QString RecordId;

    //wartosci liczbowe z dwoch elektrod
    QVector<double> *primary;
    QVector<double> *secondary;

    //przefiltrowany sygnal ekg - wyjscie modulu ECG_BASELINE
    QVector<double> *ecg_baselined;
    QVector<QPointF> *characteristics;

    //numery probek zalamkow R - wyjscie modulu R_PEAKS
    QVector<QVector<double>::const_iterator> *Rpeaks;
    QVector<unsigned int> Rpeaks_uint;

    //punkty charakterystyczne - wyjscie modulu WAVES
    // EcgFrame zawiera punkty charakterystyczne: QRS_onset, QRS_end, T_end, P_onset, P_end
        //na razie wrzuce osobno PWaveStart, ale docelowo ladniej by bylo miec to w jednej klasie jak wyzej
    Waves_struct *Waves;
    //QVector<QVector<double>::const_iterator> *PWaveStart;


    //Wykryte klasy zespolu QRS - wyjscie modulu QRS_CLASS
    QVector<QRSClass>* classes;

    // modul ST_INTERVAL
    QList<EcgStDescriptor> *STintervals;

    // HRV1
    //dane statystyczne
    double Mean, SDNN, RMSSD, RR50, RR50Ratio, SDANN, SDANNindex, SDSD;
    QList<double> *RR_x;
    QList<double> *RR_y;

    //do interpolacji
    QList<double> fftSamplesX;
    QList<double> fftSamplesY;
    QList<double> interpolantX;
    QList<double> interpolantY;

    //dane czestotliwosciowe
    double TP, HF, LF, VLF, ULF, LFHF;
    QVector<double> *fft_x;
    QVector<double> *fft_y;

    //dane histogramu, dane wykresu Poincare - wyjscie modulu HRV2
    QList<unsigned int> *histogram_x, *poincare_x;
    QList<int> *histogram_y, *poincare_y;
    double *triangularIndex, *TINN, *SD1, *SD2;

    //wartosc TWA
    QList<double> *TWA_positive_value;
    QList<unsigned int> *TWA_positive;
    QList<double> *TWA_negative_value;
    QList<unsigned int> *TWA_negative;
    unsigned int *twa_num; double *twa_highest_val; unsigned char TWA_mode;

    //zmienne dla modulu DFA
     QList<double> *trend_y, *trend_z;
     QList<double> *trend_x, *trend_v;
     int *window_min, *window_max,  *boxes, *window_plot;
     double *alfa, *wsp_a, *wsp_b;

    //modul HRT
    double *turbulence_slope, *turbulence_onset;
    int *vpbs_detected_count;
    QList<double> *hrt_tachogram;

    //modul ATRIAL_FIBR
    double PWaveOccurenceRatio;
    double RRIntEntropy;
    double RRIntDivergence;
    bool   AtrialFibr;

    //modul SigEdr
    QVector<QVector<double>::const_iterator> *SigEdr;


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
