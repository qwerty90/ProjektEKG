#ifndef SIG_EDR_H
#define SIG_EDR_H

#pragma once

#include <QList>
#include <QVector>
#include <QString>

#include <QsLog.h>
#include <QsLogDest.h>
#include <QDir>

#include <vector>
#include <cmath>

class sig_edr{

const QVector<double> signal_one; //sygnal EKG z pierwszej elektrody
const QVector<double> signal_two; //sygnal EKG z drugiej elektrody

//Qvector wartosci sygnalu EDR z pierwszej elektrody z wykorzystaniem RPEAKS
QVector<double> EDRsignal_RPeaks_one;
//Qvector wartosci sygnalu EDR z drugiej elektrody z wykorzystaniem RPEAKS
QVector<double> EDRsignal_RPeaks_two;
//Qvector wartosci sygnalu EDR z wykorzystaniem WAVES
QVector<double> EDRsignal_Waves;
//std::vector wartosci sygnalu EDR z dowolnej metody
std::vector<double> EDRsignal;

//Qvector wartosci calek kolejnych kompleksow QRS sygnalu pierwszego
QVector<double> Integrals_one;
//Qvector wartosci calek kolejnych kompleksow QRS sygnalu drugiego
QVector<double> Integrals_two;


public:
    //konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod
    //do odpowiednich pol w obiekcie
    sig_edr(const QVector<double> &signal_one,
            const QVector<double> &signal_two);

    //konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod
    //i iteratory R-pikow do odpowiednich pol w obiekcie
    sig_edr(const QVector<double> &signal_one,
            const QVector<unsigned int> &RPeaksIterators_one,
            const QVector<double> &signal_two,
            const QVector<unsigned int> &RPeaksIterators_two
            );

    //konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod,
    //iteratory QRSonset i QRSend do odpowiednich pol w obiekcie
    sig_edr(const QVector<double> &signal_one,
            const QVector<double>::const_iterator &QRSonsetIterators_one,
            const QVector<double>::const_iterator &QRSendIterators_one,
            const QVector<double> &signal_two,
            const QVector<double>::const_iterator &QRSonsetIterators_two,
            const QVector<double>::const_iterator &QRSendIterators_two
            );

    //funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu RPEAKS
    //przyjmuje numer sygnalu oraz iteratory kolejnych R-pikow w tym sygnale
    void new_RPeaks_signal(int signal_num,
                           const QVector<unsigned int> &RPeaksIterators
                           );

    //funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu WAVES
    //przyjmuje iteratory kolejnych punktow QRSonset i QRSend z obu sygnalow
    void new_Waves_signal(const QVector<double>::const_iterator &QRSonsetIterators_one,
                          const QVector<double>::const_iterator &QRSendIterators_one,
                          const QVector<double>::const_iterator &QRSonsetIterators_two,
                          const QVector<double>::const_iterator &QRSendIterators_two
                          );

    //funkcja zwracajaca sygnal EDR jako QVector
    //zarowno z jednej jak i z drugiej metody
    QVector<double>* retrieveEDR_QVec(int EDR_type, int signal_num);

    //funkcja zwracajaca sygnal EDR jako std::vector zamiast QVector
    //zarowno z jednej jak i z drugiej metody
    std::vector<double>* retrieveEDR_StdVec(int EDR_type, int signal_num);

private:

    //funkcja obliczajaca pole powierzchni kompleksu QRS
    //czyli srednia wartosc sygnalu miedzy QRS_onset a QRS_end
    double integral(QVector<double> &value);

    //funkcja obliczajaca wartosc sygnalu oddechowego na podstawie
    //sygnalu EKG z dwoch elektrod - metoda wykorzystujaca modul WAVES
    void calculate_signal_from_QRS(const QVector<double> &QRSIntegrals_one,
                                   const QVector<double> &QRSIntegrals_two);
};

#endif //SIG_EDR_H
