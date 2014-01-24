#include "waves.h"

using namespace std;

void waves::calculate_waves(QVector<double>& ecg,vector_it& r_peaks, double fs_in)
{
    fs=fs_in;
    set_qrs_onset(ecg,r_peaks);
    set_qrs_end(ecg,r_peaks);
    set_p_onset(ecg,r_peaks);
    set_p_end(ecg,r_peaks);
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

void waves::set_p_onset(QVector<double>& ecg, vector_it& r_peaks)
{
    double Rv;
    double mediana=0;
    double Pmax=0;
    int P_mid;
    int ipeak;
    QVector<int> P_oneset;
    QVector<int> P_middle;
    QVector<int> window_length;
    QVector<double> PToneset;
    QVector<double> dPToneset;
    QVector<double> window_signal;
    QVector<double> onset_signal;
    QVector<double> Pt;
    QVector<double> M;
    int p_one_end_window=20;
    for(int i=0;i<r_peaks.size()-1;i++)
    {

        window_length.push_back(ceil(0.25*((r_peaks.at(i+1)-ecg.begin())-(r_peaks.at(i)-ecg.begin()))));
    }
    for(int i=0;i<window_length.size()-1;i++)
    {
        Rv=0.003;
        for(int j=0;j<window_length.at(i);j++)
        {
            window_signal.push_back(ecg.at(qrs_onset_it.at(i+1)-ecg.begin()-window_length.at(i)-1+j));
        }
        for(int j=0;j<window_signal.size();j++)
        {
            mediana+=window_signal.at(j);
        }
        mediana/=window_length.at(i);
        for(int j=0;j<window_signal.size();j++)
        {
            window_signal.replace(j,window_signal.at(j)-mediana);
            Pt.push_back(atan(window_signal.at(j)/Rv));
            M.push_back(sqrt(pow(Rv,2)+ pow(window_signal.at(j),2)));
        }
        mediana=0;
        for(int j=0;j<Pt.size();j++)
        {
            if(Pmax<=Pt.at(j))
            {
                Pmax=Pt.at(j);
                ipeak=j;
            }
        }

        if (ipeak==0) ipeak=1;                      // Umozliwiają przejscie algorytmu w przypadkach krytycznych by TTDW
        if (ipeak==M.size()-1) ipeak=M.size()-2;    // Jeszcze będzie poprawiane
        if((M.at(ipeak-1)<M.at(ipeak))&&(M.at(ipeak)>M.at(ipeak+1)))
        {
            P_mid=ipeak+(qrs_onset_it.at(i+1)-ecg.begin()-window_length.at(i));
            P_middle.push_back(P_mid);
            Rv=0.005;
            for(int j=P_mid-p_one_end_window;j<P_mid;j++)
            {
                onset_signal.push_back(ecg.at(j));
                mediana+=onset_signal.last();
            }
            mediana/=onset_signal.size();
            for(int j=0;j<onset_signal.size();j++)
                onset_signal.replace(j,onset_signal.at(j)-mediana);
            for(int j=0;j<onset_signal.size();j++)
            {
                PToneset.push_back(atan(onset_signal.at(j)/Rv));
                if(j==0)
                    continue;
                else
                {
                    dPToneset.push_back(PToneset.at(j)-PToneset.at(j-1));
                    if(dPToneset.last()>0.01)
                    {
                        P_oneset.push_back(P_mid-p_one_end_window+j-1);
                        p_onset_it.push_back(ecg.begin()+P_mid-p_one_end_window+j-1);
                        break;
                    }
                }
            }
        }
        window_signal.clear();
        onset_signal.clear();
        Pt.clear();
        M.clear();
        PToneset.clear();
        dPToneset.clear();
        Pmax=0;
    }

}
void waves::set_p_end(QVector<double>& ecg,vector_it& r_peaks)
{
    double Rv;
    double mediana=0;
    double Pmax=0;
    int P_mid;
    int ipeak;
    QVector<int> P_end;
    QVector<int> P_middle;
    QVector<int> window_length;
    QVector<double> PTend;
    QVector<double> dPTend;
    QVector<double> window_signal;
    QVector<double> onset_signal;
    QVector<double> Pt;
    QVector<double> M;

    int p_one_end_window=20;
    for(int i=0;i<r_peaks.size()-1;i++)
    {
        window_length.push_back(ceil(0.25*((r_peaks.at(i+1)-ecg.begin())-(r_peaks.at(i)-ecg.begin()))));
    }
    for(int i=0;i<window_length.size()-1;i++)
    {
        Rv=0.003;

        for(int j=0;j<window_length.at(i);j++)
        {
            window_signal.push_back(ecg.at(qrs_onset_it.at(i+1)-ecg.begin()-window_length.at(i)-1+j));
        }
        for(int j=0;j<window_signal.size();j++)
        {
            mediana+=window_signal.at(j);
        }
        mediana/=window_length.at(i);
        for(int j=0;j<window_signal.size();j++)
        {
            window_signal.replace(j,window_signal.at(j)-mediana);
            Pt.push_back(atan(window_signal.at(j)/Rv));
            M.push_back(sqrt(pow(Rv,2)+ pow(window_signal.at(j),2)));
        }
        mediana=0;
        for(int j=0;j<Pt.size();j++)
        {
            if(Pmax<=Pt.at(j))
            {
                Pmax=Pt.at(j);
                ipeak=j;
            }
        }
        if (ipeak==0) ipeak=1;                          //analogicznie jak w p_onset
        if (ipeak==M.size()-1) ipeak=M.size()-2;        //
        if((M.at(ipeak-1)<M.at(ipeak))&&(M.at(ipeak)>M.at(ipeak+1)))
        {
            P_mid=ipeak+(qrs_onset_it.at(i+1)-ecg.begin()-window_length.at(i));
            P_middle.push_back(P_mid);
            Rv=0.005;
            for(int j=P_mid;j<P_mid+p_one_end_window;j++)
            {
                onset_signal.push_back(ecg.at(j));
                mediana+=onset_signal.last();
            }
            mediana/=onset_signal.size();

            for(int j=0;j<onset_signal.size();j++)
            {
                onset_signal.replace(j,onset_signal.at(j)-mediana);
                PTend.push_back(atan(onset_signal.at(j)/Rv));
            }
            for(int j=onset_signal.size()-1;j>0;j--)
            {
                if(j==(onset_signal.size()-1))
                    continue;
                else
                {

                    dPTend.push_front(PTend.at(j)-PTend.at(j+1));



                    if(dPTend.first()>0.01)
                    {
                        P_end.push_back(P_mid+j);

                        p_end_it.push_back(ecg.begin()+P_mid+j);
                        break;
                    }
                }
            }}
        window_signal.clear();
        onset_signal.clear();
        Pt.clear();
        M.clear();
        PTend.clear();
        dPTend.clear();
        Pmax=0;
    }
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
