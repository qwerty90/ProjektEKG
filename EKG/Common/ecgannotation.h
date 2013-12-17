#ifndef ECGANNOTATION_H
#define ECGANNOTATION_H

#include <QObject>

//Dodatkowe informacje na temat sygnalu,
//http://physionet.cps.unizar.es/physiobank/annotations.shtml#aux
struct EcgAnnotation
{
public:

    //czas od rozpoczecia pomiaru
    QString time;

    //id probki
    int sampleId;

    //typ odnotowanego zdarzenia
    QChar type;

    QChar sub;
    QChar chan;
    QChar num;

    //bardziej precyzyjny opis zdarzenia
    QString aux;

    explicit EcgAnnotation(QObject *parent = 0);
    EcgAnnotation(QString t, int sid, QChar tp);
    
signals:
    
public slots:
    
};

#endif // ECGANNOTATION_H
