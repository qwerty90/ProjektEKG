#ifndef T_WAVE_ALT_H
#define T_WAVE_ALT_H

#include <QtCore/qglobal.h>
#include <QList>

#if defined(T_WAVE_ALT_LIBRARY)
#  define T_WAVE_ALTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define T_WAVE_ALTSHARED_EXPORT Q_DECL_IMPORT
#endif

class T_WAVE_ALTSHARED_EXPORT T_WAVE_ALT
{
public:

    struct Extremum {
        unsigned int index;
        double value;
    };

    double countMean(QList<double>*, unsigned int);
    Extremum countExtremum(QList<double>*, unsigned int, unsigned int, bool);
    int abs(int);
    double abs(double);

    int window_size;
    void Process();
    int mode;
    QList<double> *baselined;
    QList<unsigned int> *t_end;
    QList<unsigned int> *qrs_end;

    QList<Extremum> *t_wave_temp;
    QList<unsigned int> *TWA_positive;
    QList<double> *TWA_positive_value;
    QList<unsigned int> *TWA_negative;
    QList<double> *TWA_negative_value;
    double *twa_highest_val;
    unsigned int *twa_num;

};

#endif
