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
    waves(){};

    void calculate_waves(QVector<double>&,vector_it&,double);
    const vector_it & get_qrs_onset();
    const vector_it & get_qrs_begin();
    const vector_it & get_p_onset();
    const vector_it & get_p_end();

private:
    void set_qrs_onset(QVector<double>&,vector_it&);
    void set_qrs_end(QVector<double>&,vector_it&);
    void set_p_onset(QVector<double>&,vector_it&);
    void set_p_end(QVector<double>&,vector_it&);
    double fs;
    vector_it qrs_onset_it;
    vector_it qrs_end_it;
    vector_it p_end_it;
    vector_it p_onset_it;

};

#endif // WAVES_H
