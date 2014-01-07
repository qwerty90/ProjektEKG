#ifndef ECGINFO_H
#define ECGINFO_H

#include <QObject>
#include <QList>

class EcgInfo : public QObject
{
    Q_OBJECT
public:
    QString medicines;
    QChar sex;
    int age;
    QString samplingFreq;
    float frequencyValue;
    QString signalLength;
    QString primaryName;
    QString secondaryName;

    explicit EcgInfo(QObject *parent = 0);

    void parseFrequency();
    
signals:
    
public slots:
    
};

#endif // ECGINFO_H
