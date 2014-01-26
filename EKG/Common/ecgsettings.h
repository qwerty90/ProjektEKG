#ifndef ECGSETTINGS_H
#define ECGSETTINGS_H

#include <QObject>

//ustawienia dla modulow zadawane z UI

class EcgSettings : public QObject
{
    Q_OBJECT
public:
    explicit EcgSettings(QObject *parent = 0);

    //raw data settings
    int signalIndex;
    //baseline settings
    int EcgBaselineMode;
    QString kalman_arg1;
    QString kalman_arg2;
    int     avgWindowSize;
    int     coeff_set; //wybrany zestaw parametrow
    //rpeaks settings
    unsigned char RPeaksMode; //1-hilb 2-pan 3-falkowa
    //waves settings
    bool P_on_checked;
    bool P_end_checked;
    bool Qrs_on_checked;
    bool Qrs_end_checked;
    //st interval setting
    bool quadratic; //0=linear
    double morph_coeff;
    double level_tresh;
    double slope_tresh;
    unsigned int detect_window;
    unsigned int smooth_window;
    //SigEdr settings
    bool SigEdr_rpeaks;
    bool SigEdr_qrs;
    int  SigEdr_lead;

signals:
    
public slots:
    
};

#endif // ECGSETTINGS_H
