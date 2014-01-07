#ifndef ECG_EXAMPLE_H
#define ECG_EXAMPLE_H

#include <QList>
#include <QVector>
#include <QString>

#include <QsLog.h>  //to warto dorzucic
#include <QsLogDest.h>
#include <QDir>

class ecg_example
{
private:
void calculate_mean();  //tu mnie nie obchodzi co macie
int window;
QString method;

QList<int>    *raw_data;
QVector<double> *filtered;
int     gain;//wzmocnienie probki

public:
    ecg_example();

    void get_params(int window_size,QString method,int gain);//tym przesle wam wszystkie potrzebne parametry
    void get_data(QList<int>* raw_data);            //tym dane
    QVector<double>* export_data();                 //tym zwrocicie wskaznik do wynikow
    void run();

};

#endif // ECG_EXAMPLE_H
