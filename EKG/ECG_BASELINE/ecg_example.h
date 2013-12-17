#ifndef ECG_EXAMPLE_H
#define ECG_EXAMPLE_H

#include <QList>
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

public:
    ecg_example();

    QList<int>    *raw_data; //to zostawcie public, to zrobie wam kopiowanie danych
    QList<double> *filtered; // dane wyjsciowe tez public

    void get_params(int window_size,QString method);//tym przesle wam wszystkie potrzebne parametry

    void run();

};

#endif // ECG_EXAMPLE_H
