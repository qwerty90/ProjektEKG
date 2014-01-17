#include "waves.h"

using namespace std;

void waves::calculate_waves(QVector<double>& ecg,vector_it& r_peaks, double fs_in)
{
    fs=fs_in;
    set_qrs_onset(ecg,r_peaks);
    set_qrs_end(ecg,r_peaks);
    set_p_onset(ecg);
    set_p_end(ecg);
}

void waves::set_qrs_onset(QVector<double>& ecg, vector_it& r_peaks)
{
    const float low_freq_lim = 0.5;
    const float hi_freq_lim = 40;

    double a_t_max, env_t=0.0, a_t=0.0;
    int window_qrs_onset=ceil(0.25*fs);
    int sliding_win_start,s2=0,tp_int=0,w=0,temp_j=0,win_end=0;
    QVector<double> envelope_win;
    it tp, envelope_end, envelope_start;
    QVector<double> signal(ecg.end()-ecg.begin()),hilbert(ecg.end()-ecg.begin());

    fft_filter(ecg.begin(), ecg.end(), signal.begin(), signal.end(),fs,low_freq_lim,hi_freq_lim);
    hilbert_transform(signal.begin(), signal.end(), hilbert.begin(), hilbert.end());
    envelope(signal.begin(), signal.end(), hilbert.begin(), hilbert.end());

    envelope_end=signal.end();
    envelope_start=signal.begin();

    for(int i=0; i<r_peaks.end()-r_peaks.begin(); i++)
    {

                 if (!((r_peaks[i]-ecg.begin())-window_qrs_onset < 0))
                 {
                    a_t_max=-INFINITY;
                    sliding_win_start=r_peaks[i]-ecg.begin()-window_qrs_onset;

                    for(int j=r_peaks[i]-ecg.begin()-1;j>=sliding_win_start;j--)
                    {
                        if (j+window_qrs_onset>envelope_end-envelope_start)
                            win_end=envelope_end-envelope_start;
                        else
                            win_end=j+window_qrs_onset;

                        envelope_win.clear();
                        a_t=0;
                        for(int k=j;k<=win_end;k++)
                            envelope_win.push_back(*(envelope_start+k));


                        env_t=*(envelope_start+j);


                        tp=(envelope_win.begin());
                        for(QVector<double>::iterator it=envelope_win.begin();it!=envelope_win.end();it++)
                        {
                            a_t+=*it-env_t;

                            if (*tp<*it)
                                tp=it;
                        }

                        if (a_t_max<a_t)
                        {
                            a_t_max=a_t;
                            s2=j;
                            tp_int=tp-envelope_win.begin()+j-1;
                        }
                    }

                    w=tp_int-s2;
                    a_t_max=-INFINITY;
                    for(int j=s2;j<=tp_int;j++)
                    {
                        a_t=0;
                        envelope_win.clear();
                        for(int k=j;k<=j+w;k++)
                            envelope_win.push_back(*(envelope_start+k));
                        env_t=*(envelope_start+j);
                        for(QVector<double>::iterator it=envelope_win.begin();it!=envelope_win.end();it++)
                        {
                            a_t+=*it-env_t;
                        }
                        if (a_t_max<a_t)
                        {
                            a_t_max=a_t;
                            temp_j=j;
                        }

                    }
                    if (temp_j!=0){
                    qrs_onset_it.push_back(ecg.begin()+temp_j);
                    }
                 }
    }
}

void waves::set_qrs_end(QVector<double>& ecg,vector_it& r_peaks)
{
    const float low_freq_lim = 5;
    const float hi_freq_lim = 30;

    double a_t_max, env_t=0.0, a_t=0.0;
    int window_qrs_end=ceil(0.1*fs);
    int sliding_win_end,s2=0,tp_int=0,w=0,temp_j=-1;
    QVector<double> envelope_win;
    it tp, envelope_end, envelope_start;
    QVector<double> signal(ecg.end()-ecg.begin()),hilbert(ecg.end()-ecg.begin());

    fft_filter(ecg.begin(), ecg.end(), signal.begin(), signal.end(),fs,low_freq_lim,hi_freq_lim);
    hilbert_transform(signal.begin(), signal.end(), hilbert.begin(), hilbert.end());
    envelope(signal.begin(), signal.end(), hilbert.begin(), hilbert.end());

    envelope_end=signal.end();
    envelope_start=signal.begin();

    for(int i=0; i<r_peaks.end()-r_peaks.begin(); i++)
    {
        if (!((r_peaks[i]-ecg.begin())+window_qrs_end > envelope_end-envelope_start))
                 {
                    a_t_max=-INFINITY;
                    sliding_win_end=(r_peaks[i]-ecg.begin())+window_qrs_end;

                    for(int j=(r_peaks[i]-ecg.begin())+1;j<=sliding_win_end;j++)
                    {
                        envelope_win.clear();
                        if (j-window_qrs_end<0)
                        {
                            for(int k=0;k<=j;k++)
                                envelope_win.push_back(*(envelope_start+k));
                        }
                        else
                        {
                            for(int k=j-window_qrs_end;k<=j;k++)
                                envelope_win.push_back(*(envelope_start+k));
                        }
                        a_t=0;
                        env_t=*(envelope_start+j);

                        tp=(envelope_win.begin());
                        for(QVector<double>::iterator it=envelope_win.begin();it!=envelope_win.end();it++)
                        {
                            a_t+=*it-env_t;
                            if (*tp<*it){
                                tp=it;
                            }
                        }

                        if (a_t_max<a_t)
                        {
                            a_t_max=a_t;
                            s2=j;
                            tp_int=tp-envelope_win.begin()+j-envelope_win.size();
                        }
                    }
                    w=s2-tp_int;
                    a_t_max=-INFINITY;
                    for(int j=tp_int;j<=s2;j++)
                    {
                        a_t=0;
                        envelope_win.clear();
                        if (j-w<0)
                        {
                            for(int k=0;k<=j;k++)
                                envelope_win.push_back(*(envelope_start+k));
                        }
                        else
                        {
                            for(int k=j-w;k<=j;k++)
                                envelope_win.push_back(*(envelope_start+k));
                        }
                        env_t=*(envelope_start+j);
                        for(QVector<double>::iterator it=envelope_win.begin();it!=envelope_win.end();it++)
                        {
                            a_t+=*it-env_t;
                        }
                        if (a_t_max<a_t)
                        {
                            a_t_max=a_t;
                            temp_j=j;
                        }
                    }
                    if (temp_j!=-1){
                    qrs_end_it.push_back(ecg.begin()+temp_j);
                    }
                 }
    }

}

void waves::set_p_onset(QVector<double>& ecg)
{


}
void waves::set_p_end(QVector<double>& ecg)
{

}

const vector_it & waves::get_qrs_onset()
{
    return qrs_onset_it;
}
const vector_it & waves::get_qrs_begin()
{
    return qrs_end_it;
}
const vector_it & waves::get_p_onset()
{
    return p_onset_it;
}
const vector_it & waves::get_p_end()
{
    return p_end_it;
}
