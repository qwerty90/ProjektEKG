#ifndef ECGSETTINGS_H
#define ECGSETTINGS_H

#include <QObject>
#include "qrsclass.h"

//ustawienia dla modulow zadawane z UI

class EcgSettings : public QObject
{
    Q_OBJECT
public:
    explicit EcgSettings(QObject *parent = 0);

    int EcgBaselineMode;
    QString kalman_arg1;
    QString kalman_arg2;
    double  averaging_time;
    int     avgWindowSize;
    unsigned char RPeaksMode;
    int signalIndex;
    
    QRSClassSettings qrsClassSettings;
signals:
    
public slots:
    
};

#endif // ECGSETTINGS_H
