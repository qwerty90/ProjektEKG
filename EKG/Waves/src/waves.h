#ifndef WAVES_H
#define WAVES_H

#include <QVector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

typedef QVector<double>::const_iterator it;
typedef QVector<it>vector_it;

void hilbert_transform(QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator);
void fft_filter(QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator, float, float, float);
void envelope(QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator, QVector<double>::iterator);

class waves
{
public:

    waves(QVector<double> & sig_ref, float fs_in): ecg(sig_ref),fs(fs_in){};

    const vector_it & get_qrs_onset(vector_it&);
    const vector_it & get_qrs_end(vector_it&);
    const vector_it & get_p_onset(vector_it&);
    const vector_it & get_p_end(vector_it&);

private:
    QVector<double> & ecg;
    float fs;

};

#endif // WAVES_H
