#ifndef SLEEP_APNEA_H
#define SLEEP_APNEA_H
#include <cmath>
#include <QVector>
#include <QPair>
#include <algorithm>
#include <numeric>
#include <QDebug>
#include <cstdlib>
typedef QPair<unsigned long int, unsigned long int> BeginEndPair;

class sleep_apnea
{
public:
    sleep_apnea(const int sampling_freqency);

    //module set properties
    int data_freq;          //sampling freqency of data (tested for 100Hz)
    int window;          //window for averange filter (set 41 !!!)
    int LFILT;           //for hilbert transform (set 32 !!! must be even !!!)
    int window_median;   //window for median filter (set 60 !!!)

    //OUTPUT FOR GUI
        //data for plots
        //QVector[0]: time in samples
        //QVector[1]: Hilbert amplitude
        //QVector[2]: Hilbert freqency
        QVector<QVector<double>> sleep_apnea_plots(QVector<unsigned int> tab_R_peaks);
        //vector of pairs:(begin and end sample of apnea detection)
        QVector<BeginEndPair> sleep_apnea_output(QVector<unsigned int> tab_R_peaks);
        //Qvector[0]: Treshold value of max_amplitude [s]
        //Qvector[1]: Treshold value of min_frequency [Hz]
        //Qvector[2]: Apnea assessment in the time domain [%]
        //Qvector[3]: Apnea assessment in the frequency domain [%]
        QVector<double> gui_output(QVector<unsigned int> tab_R_peaks);
private:
    //basic functions
    QVector<QVector<double>> RR_intervals(QVector<unsigned int> tab_R_peaks);//tab_R_peaks MUST BE CONNECTED WITH OUTPUT OF RPEAKS MODULE!!
    QVector<QVector<double> > averange_filter(QVector<QVector<double> > tab_RR);
    QVector<QVector<double> > resample(QVector<QVector<double> > tab_RR_new);
    void HP_LP_filter (QVector<QVector<double> > &tab_res);
    void hilbert(QVector<QVector<double> > tab_res, QVector<QVector<double> > &h_amp, QVector<QVector<double> > &h_freq);
    void freq_amp_filter (QVector<QVector<double> > &h_freq, QVector<QVector<double> > &h_amp);
    void median_filter (QVector<QVector<double> > &h_freq, QVector<QVector<double> > &h_amp);
    //final function
    QVector<BeginEndPair> apnea_detection(QVector<QVector<double> > tab_amp,QVector<QVector<double> > tab_freq);
};

#endif // SLEEP_APNEA_H
