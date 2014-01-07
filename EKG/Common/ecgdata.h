#ifndef ECGDATA_H
#define ECGDATA_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QMap>
#include "ecgannotation.h"
#include "ecginfo.h"
#include "ecgsettings.h"
#include "waves.h"
#include "qrsclass.h"

class EcgData : public QObject
{
    Q_OBJECT
public:
    QString RecordId;

    //wartosci czasu dla kolejnych pomiarow
    QList<QString> *time;

    //wartosci liczbowe z dwoch elektrod
    QList<int> *primary;
    QList<int> *secondary;

    //przefiltrowany sygnal ekg - wyjscie modulu ECG_BASELINE
    const QVector<double> *ecg_baselined;

    //numery probek zalamkow R - wyjscie modulu R_PEAKS
    const QVector<QVector<double>::const_iterator> *Rpeaks;

    //punkty charakterystyczne - wyjœcie modulu WAVES
    // EcgFrame zawiera punkty charakterystyczne: QRS_onset, QRS_end, T_end, P_onset, P_end
    QList<Waves::EcgFrame*> *waves;
    //na razie wrzuce osobno PWaveStart, ale docelowo ladniej by bylo miec to w jednej klasie jak wyzej
    const QVector<QVector<double>::const_iterator> *PWaveStart;


    //Wykryte klasy zespo³u QRS - wyjœcie modu³u QRS_CLASS
    QList<QRSClass> *classes;

    //STInterval
    QList<int> *STbegin_x_probki;
    QList<int> *STend_x_probki;

    QList<double> *STbegin_x;
    QList<double> *STend_x;

    QList<double> *STbegin_y;
    QList<double> *STend_y;

    QList<double> *ecg_baselined_mv;

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
    QList<double> *fft_x;
    QList<double> *fft_y;

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

    QList<EcgAnnotation> *annotations;
    EcgInfo *info;

    EcgSettings *settings;

    EcgData(QString id);

    explicit EcgData(QObject *parent = 0);

    QList<int>* GetCurrentSignal();
    
signals:
    
public slots:
    
};

#endif // ECGDATA_H
