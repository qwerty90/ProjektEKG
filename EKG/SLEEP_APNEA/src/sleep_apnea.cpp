#include "sleep_apnea.h"

sleep_apnea::sleep_apnea(const int sampling_freqency)
    : data_freq(sampling_freqency),
      window(41),
      LFILT(32),
      window_median(60)
{ }

QVector<QVector<double>> sleep_apnea::RR_intervals(QVector<unsigned int> tab_R_peaks)
{
    //contruct 2D array
    QVector<QVector<double> > tab_RR(2);
    for(int outer = 0; outer < tab_RR.size(); ++outer) tab_RR[outer].resize(tab_R_peaks.size());

    //fill array
    int i;
    for(i=0; i<tab_R_peaks.size(); i++){
        //sample number
        tab_RR[0][i]=double(tab_R_peaks[i]);
    }
    for(i=0; i<tab_R_peaks.size()-1; i++){
        //rr_intervals in seconds
        tab_RR[1][i]=double(double(tab_R_peaks[i+1])-double(tab_R_peaks[i]))/data_freq;
    }

    return tab_RR;
}

QVector<QVector<double>> sleep_apnea::averange_filter(QVector<QVector<double>> tab_RR)
{
    int length=tab_RR[0].size();
    bool correct[length];

    for(int i = 0;i<(int)floor(length/window)*window;i++)
    {
        int k = (int)floor(i/(window+1));

        //calculating local mean
        double sum = 0;
        int values_cnt=0;
        for(int z = 0+window*k;z<window+window*k;z++)
        {
            if (z != i && tab_RR[1][z]>0.4 && tab_RR[1][z]<2.0)
            {
                sum = sum + tab_RR[1][z];
                values_cnt++;
            }
        }
        //deciding if tab_RR[1][i] is correct
        double mean = sum/values_cnt;
        if(tab_RR[1][i]>0.8*mean && tab_RR[1][i]<1.2*mean)
            correct[i] =true;
        else
            correct[i] =false;
    }

    //loop for end samples
    if(length%window==0)
    {
        int h =floor(length/window)*window;
        for(int i = h;i<length;i++)
        {
            //calculating local mean
            double sum = 0;
            int values_cnt=0;
            for(int z=h;z<length;z++)
            {
                if (z != i && tab_RR[1][z]>0.4 && tab_RR[1][z]<2.0)
                {
                    sum = sum + tab_RR[1][z];
                    values_cnt++;
                }
            }
            //deciding if tab_RR[1][i] is correct
            double mean = sum/values_cnt;
            if(tab_RR[1][i]>0.8*mean && tab_RR[1][i]<1.2*mean)
                correct[i] =true;
             else
                correct[i] =false;
        }
    }

    //generating new table
    int correct_cnt=0;
    for(int i=0;i<length;i++)
    {
        if(correct[i]==true)
        correct_cnt++;
    }
    QVector<QVector<double> > tab_RR_new(2);
    for(int outer = 0; outer < tab_RR_new.size(); ++outer) tab_RR_new[outer].resize(correct_cnt);
    //filling new table
    int j=0;
    for(int i=0;i<length;i++)
    {
        if(correct[i]==true)
        {
            tab_RR_new[0][j]=tab_RR[0][i];
            tab_RR_new[1][j]=tab_RR[1][i];
            j++;
        }
    }
    return tab_RR_new;
}

QVector<QVector<double>> sleep_apnea::resample(QVector<QVector<double>> tab_RR_new)
{
    int n_start=tab_RR_new[0][1];
    int n_stop=tab_RR_new[0][tab_RR_new[0].size()-1];
    int size1=ceil((n_stop-n_start)/data_freq)+1;
    //generating output array
    QVector<QVector<double> > tab_res(2);
    for(int outer = 0; outer < tab_res.size(); ++outer) tab_res[outer].resize(size1);
    //filling output array with samples
    int j=n_start;
    for(int i=0;i<size1;i++)
    {
        tab_res[0][i]=j;
        j+=data_freq;
    }

    //calculations for first sample in array (tab_res[1][0])
    double tm1=tab_RR_new[0][0];
    double tm2=tab_RR_new[0][1];
    double rr1=tab_RR_new[1][0];
    double rr2=tab_RR_new[1][1];
    double a=(rr1-rr2)/(tm1-tm2);
    double b=rr1-a*tm1;
    tab_res[1][0] = a*tab_res[0][0]+b;

    //calculations for last sample in array (tab_res[1][tab_RR_new.size()-1])
    if (tab_RR_new[1][tab_RR_new[0].size()-1]==0)
        tab_RR_new[1][tab_RR_new[0].size()-1]=tab_RR_new[1][tab_RR_new[0].size()-2];
    tm1=tab_RR_new[0][tab_RR_new[0].size()-2];
    tm2=tab_RR_new[0][tab_RR_new[0].size()-1];
    rr1=tab_RR_new[1][tab_RR_new[0].size()-2];
    rr2=tab_RR_new[1][tab_RR_new[0].size()-1];
    a=(rr1-rr2)/(tm1-tm2);
    b=rr1-a*tm1;
    tab_res[1][size1-1] = a*tab_res[0][size1-1]+b;


    //calculations for 1..last-1 samples
    for(int j=1;j<size1-1;j++)
    {
        //finding the closest left neighbour for tab_res[0,j]
        int i = 0;
        while(i<tab_RR_new[0].size()-2 && tab_RR_new[0][i]<tab_res[0][j])
        {
            if(tab_RR_new[0][i+1]>tab_res[0][j])
                 break;
            else
                 i=i+1;
         }
        //qDebug() <<"j="<< j <<"("<<size1-1<<")"<<" i="<<i<<"("<<tab_RR_new[0].size()-1<<")"<<"     "<<tab_RR_new[0][i]<<"<"<<tab_res[0][j]<<"<"<<tab_RR_new[0][i+1];
         tm1=tab_RR_new[0][i];
         tm2=tab_RR_new[0][i+1];
         rr1=tab_RR_new[1][i];
         rr2=tab_RR_new[1][i+1];
         a=(rr1-rr2)/(tm1-tm2);
         b=rr1-a*tm1;
         tab_res[1][j] = a*tab_res[0][j]+b;

    }

    return tab_res;
}

void sleep_apnea::HP_LP_filter (QVector<QVector<double> > &tab_res)
{
    int i,j;
    //high-pass filter
    QVector<double> Z1(tab_res[0].size());
    for(i=0;i<Z1.size();i++)Z1[i]=0;

    double CUTOFF = 0.01;
    double RC = 1.0/(CUTOFF*2*3.14);
    double dt = 1.0/1.0;
    double alpha = RC/(RC+dt);

    for (j=1;j<tab_res[0].size();j++)
        Z1[j]=alpha*(Z1[j-1] + tab_res[1][j] - tab_res[1][j-1]);

    //low-pass filter
    QVector<double> Z2(tab_res[0].size());
    for(int i=0;i<Z2.size();i++)Z2[i]=0;

    CUTOFF = 0.09;
    RC = 1.0/(CUTOFF*2*3.14);
    dt = 1.0/1.0;
    alpha = dt/(RC+dt);

    for (j=1;j<Z1.size();j++)
        Z2[j] = Z2[j-1] + (alpha * (Z1[j] - Z2[j-1]));

    //wrtiting to output array
    for (i=0;i<tab_res[0].size();i++)
        tab_res[1][i]=Z2[i];
}

void sleep_apnea::hilbert(QVector<QVector<double> > tab_res, QVector<QVector<double> > &h_amp, QVector<QVector<double> > &h_freq)
{
    int i,l,npt,lfilt, LMAX=tab_res[0].size()-1;
    //defining local arrays
    double x[LMAX+1], xh[LMAX+1], phase[LMAX+1],ampl[LMAX+1],time[LMAX+1],freq[LMAX+1];
    double hilb[LFILT+1], pi, pi2, xt, xht;

    pi = 3.1415;pi2 = 2*pi;lfilt = LFILT;

    for (i=1; i<=lfilt; i++)
        hilb[i]=1/((i-lfilt/2)-0.5)/pi;

    for (i=1; i<=LMAX ; i++)
    {
        time[i]=tab_res[0][i];
        x[i]=tab_res[1][i];
        xh[i] = 0.0;
        ampl[i] = 0.0;
    }
    npt=LMAX+1;

    //hilbert transform
    double yt;
    for (l=1; l<=npt-lfilt+1; l++)
    {
        yt = 0.0;
        for (i=1; i<=lfilt; i++)
            yt = yt+x[l+i-1]*hilb[lfilt+1-i];
        xh[l] = yt;
    }
        /* shifting lfilt/1+1/2 points */
    for (i=1; i<=npt-lfilt; i++) {
        xh[i] = 0.5*(xh[i]+xh[i+1]);
    }
    for (i=npt-lfilt; i>=1; i--)
        xh[i+lfilt/2]=xh[i];
        /* writing zeros */
    for (i=1; i<=lfilt/2; i++) {
        xh[i] = 0.0;
        xh[npt+1-i] = 0.0;
    }


    // Ampl and phase
    for (i=lfilt/2+1; i<=npt-lfilt/2; i++)
    {
        xt = x[i];
        xht = xh[i];
        ampl[i] = sqrt(xt*xt+xht*xht);
        phase[i] = atan2(xht ,xt);
        if (phase[i] < phase[i-1])
            freq[i] = phase[i]-phase[i-1]+pi2;
        else
            freq[i] = phase[i]-phase[i-1];
    }


    //writing output arrays
    int id_start=(LFILT/2)+1;
    int id_stop=LMAX-(LFILT/2);
    int size1=id_stop-id_start+1;
    //resizing
    for(int outer = 0; outer < h_amp.size(); ++outer)
    {
        h_amp[outer].resize(size1);
        h_freq[outer].resize(size1);
    }
    //writing time and values
    int j=0;
    for(int i=id_start;i<=id_stop;i++)
    {
        if(j<size1)
        {
        h_amp[0][j]=time[i];h_amp[1][j]=ampl[i];
        h_freq[0][j]=time[i];h_freq[1][j]=freq[i];
        j++;
        }
    }
}

void sleep_apnea::freq_amp_filter (QVector<QVector<double> > &h_freq, QVector<QVector<double> > &h_amp)
{
    double diff,t1,t2,f1,f2,a,b; int i;

    double maxi=0;
    //calculating treshold value for filter
    for (i=0;i<h_freq[0].size()-1;i++)
    {
        diff=abs(h_freq[1][i]-h_freq[1][i+1]);
        if (diff>maxi)
            maxi=diff;
    }
    double limit= maxi*0.2;


    for (i=1;i<h_freq[0].size()-1;i++)
    {
        if (abs(h_freq[1][i])>limit)
        {
            t1=h_freq[0][i-1];f1=h_freq[1][i-1];
            t2=h_freq[0][i+1];f2=h_freq[1][i+1];
            a=(f1-f2)/(t1-t2);
            b=f1-a*t1;
            h_freq[1][i]=a*h_freq[0][i]+b;
        }
    }

    //normalization of amplitude signal
    double sum=0;
    for (i=0;i<h_amp[0].size();i++)
        sum+=h_amp[1][i];
    double mean=sum/h_amp[0].size();
    //writing values to output array
    for (i=0;i<h_amp[0].size();i++)
        h_amp[1][i]=h_amp[1][i]*(1/mean);

}

void sleep_apnea::median_filter (QVector<QVector<double> > &h_freq, QVector<QVector<double> > &h_amp)
{

    QVector<double> amp(window_median);
    QVector<double> freq(window_median);
    int i,j,k,l; double median_amp,median_freq;

    for (i=0;i<h_freq[0].size();i++)
    {
        if (i%window_median==0 && i>0)
        {
            k=(i/window_median)-1;
            //filling arrays
            l=0;
            for (j=0+window_median*k;j<window_median+window_median*k;j++)
            {
                freq[l]=h_freq[1][j];
                amp[l]=h_amp[1][j];
                l++;
            }
            //sorting arrays
            std::sort(freq.begin(),freq.end());
            std::sort(amp.begin(),amp.end());
            //finding median_elements
            if (freq.size()%2!=0)
            {
                median_freq=freq[int((freq.size()-1)/2)];
                median_amp=amp[int((amp.size()-1)/2)];
            }
            else
            {
                median_freq=(freq[int(floor((freq.size()-1)/2))]+ freq[int(floor((freq.size()-1)/2))+1] )*0.5;
                median_amp=(amp[int(floor((amp.size()-1)/2))]+ amp[int(floor((amp.size()-1)/2))+1] )*0.5;
            }
            //writing output arrays
            for (j=0+window_median*k;j<window_median+window_median*k;j++)
            {
                h_freq[1][j]=median_freq;
                h_amp[1][j]=median_amp;
            }
        }
    }

    //loop for last elements
    if (h_freq[0].size()%window_median!=0)
    {
        int start_id=int(floor(h_freq[0].size()/window_median)*window_median);
        int stop_id=h_freq[0].size()-1;
        QVector<double> amp1(stop_id-start_id+1);
        QVector<double> freq1(stop_id-start_id+1);
        //filling arrays
        j=start_id;
        for(i=0;i<freq1.size();i++)
        {
            freq1[i]=h_freq[1][j];
            amp1[i]=h_amp[1][j];
            j++;
        }
        //sorting arrays
        std::sort(freq1.begin(),freq1.end());
        std::sort(amp1.begin(),amp1.end());
        //finding median_elements
        if (freq1.size()%2!=0)
        {
            median_freq=freq1[int((freq1.size()-1)/2)];
            median_amp=amp1[int((amp1.size()-1)/2)];
        }
        else
        {
            median_freq=(freq1[int(floor((freq1.size()-1)/2))]+ freq1[int(floor((freq1.size()-1)/2))+1] )*0.5;
            median_amp=(amp1[int(floor((amp1.size()-1)/2))]+ amp1[int(floor((amp1.size()-1)/2))+1] )*0.5;
        }
        //writing output arrays
        for(i=start_id;i<=stop_id;i++)
        {
            h_freq[1][i]=median_freq;
            h_amp[1][i]=median_amp;
        }
    }

}

QVector<BeginEndPair> sleep_apnea::apnea_detection(QVector<QVector<double> > tab_amp,QVector<QVector<double> > tab_freq)
{
    QVector<BeginEndPair>apnea_out;

    //treshold value for amplitude
    int i; double a,b,mini_amp,mini_freq,maxi_amp,maxi_freq,mid,y_amp,y_freq;
    mini_amp=99999;mini_freq=99999;
    maxi_amp=0;maxi_freq=0;
    for(i=0;i<tab_amp[0].size();i++)
    {
        if(tab_amp[1][i]>maxi_amp)maxi_amp=tab_amp[1][i];
        if(tab_amp[1][i]<mini_amp)mini_amp=tab_amp[1][i];
        if(tab_freq[1][i]>maxi_freq)maxi_freq=tab_freq[1][i];
        if(tab_freq[1][i]<mini_freq)mini_freq=tab_freq[1][i];
    }
    a=-0.18;b=1;mid=(maxi_amp+mini_amp)*0.5;
    y_amp=a+b*(mid+1)*0.5;

    //treshold value for frequency
    y_freq=(maxi_freq+mini_freq)*0.4;

    //apnea detection
    QVector<bool>detect(tab_amp[0].size());
    for(i=0;i<tab_amp[0].size();i++)
    {
        if(tab_amp[1][i]>=y_amp)detect[i]=true;
        else detect[i]=false;
    }
    BeginEndPair pair;bool added=false;
    for(i=0;i<tab_amp[0].size();i++)
    {
        if(detect[i]==true && added==false)
        {
            pair.first=tab_amp[0][i];
            added=true;
        }
        if(((detect[i]==false) && (added==true)) || (i==tab_amp[0].size()-1))
        {
            pair.second=tab_amp[0][i];
            apnea_out.append(pair);
            pair.first=0;pair.second=0;
            added=false;
        }
    }

    if (apnea_out.size()==0) apnea_out.append(BeginEndPair(0,0));

    return apnea_out;
}


/*************************************************
********************FOR GUI***********************
*************************************************/
QVector<QVector<double>> sleep_apnea::sleep_apnea_plots(QVector<unsigned int> tab_R_peaks)
{
    //getting data
    QVector<QVector<double>>tab_RR,tab_RR_new,tab_res;
    QVector<QVector<double>>h_amp(2);
    QVector<QVector<double>>h_freq(2);
    QVector<QVector<double>> apnea_plots(3);

    tab_RR=RR_intervals(tab_R_peaks);
    tab_RR_new=averange_filter(tab_RR);
    tab_res=resample(tab_RR_new);
    HP_LP_filter(tab_res);
    hilbert(tab_res,h_amp,h_freq);
    freq_amp_filter(h_freq,h_amp);
    median_filter(h_freq,h_amp);

    //resizing output
    apnea_plots[0].resize(h_amp[0].size());
    apnea_plots[1].resize(h_amp[1].size());
    apnea_plots[2].resize(h_freq[1].size());
    //writing output
    int i;
    for(i=0;i<apnea_plots[0].size();i++)apnea_plots[0][i]=h_amp[0][i];
    for(i=0;i<apnea_plots[1].size();i++)apnea_plots[1][i]=h_amp[1][i];
    for(i=0;i<apnea_plots[2].size();i++)apnea_plots[2][i]=h_freq[1][i];
    return apnea_plots;
}

QVector<BeginEndPair> sleep_apnea::sleep_apnea_output(QVector<unsigned int> tab_R_peaks)
{
    //getting data
    QVector<QVector<double>>tab_RR,tab_RR_new,tab_res;
    QVector<QVector<double>>h_amp(2);
    QVector<QVector<double>>h_freq(2);
    QVector<BeginEndPair> apnea_output;

    tab_RR=RR_intervals(tab_R_peaks);
    tab_RR_new=averange_filter(tab_RR);
    tab_res=resample(tab_RR_new);
    HP_LP_filter(tab_res);
    hilbert(tab_res,h_amp,h_freq);
    freq_amp_filter(h_freq,h_amp);
    median_filter(h_freq,h_amp);
    apnea_output=apnea_detection(h_amp,h_freq);

    //writing output
    return apnea_output;
}

QVector<double> sleep_apnea::gui_output(QVector<unsigned int> tab_R_peaks)
{
    //getting data
    QVector<QVector<double>>tab_RR,tab_RR_new,tab_res;
    QVector<QVector<double>>h_amp(2);
    QVector<QVector<double>>h_freq(2);
    QVector<BeginEndPair> apnea_output;

    tab_RR=RR_intervals(tab_R_peaks);
    tab_RR_new=averange_filter(tab_RR);
    tab_res=resample(tab_RR_new);
    HP_LP_filter(tab_res);
    hilbert(tab_res,h_amp,h_freq);
    freq_amp_filter(h_freq,h_amp);
    median_filter(h_freq,h_amp);
    apnea_output=apnea_detection(h_amp,h_freq);


    //treshold_amp value for amplitude
    int i; double a,b,mini_amp,mini_freq,maxi_amp,maxi_freq,mid,treshold_amp,treshold_freq;
    mini_amp=99999;mini_freq=99999;
    maxi_amp=0;maxi_freq=0;
    for(i=0;i<h_amp[0].size();i++)
    {
        if(h_amp[1][i]>maxi_amp)maxi_amp=h_amp[1][i];
        if(h_amp[1][i]<mini_amp)mini_amp=h_amp[1][i];
        if(h_freq[1][i]>maxi_freq)maxi_freq=h_freq[1][i];
        if(h_freq[1][i]<mini_freq)mini_freq=h_freq[1][i];
    }
    a=-0.18;b=1;mid=(maxi_amp+mini_amp)*0.5;
    treshold_amp=a+b*(mid+1)*0.5;

    //treshold_freq value for frequency
    treshold_freq=(maxi_freq+mini_freq)*0.4;

    //assessment_amp
    int n_samples=0,n_apnea=0;double assessment_amp;
    n_samples=double(h_amp[0][h_amp[0].size()-1]-h_amp[0][0])/data_freq;
    for(int i=0;i<apnea_output.size();i++)
    {n_apnea+=apnea_output[i].second-apnea_output[i].first;}
    n_apnea=double(n_apnea)/data_freq;
    assessment_amp=(double(n_apnea)/n_samples)*100; //*100 -> value in percent

    //assessment_freq
    double assessment_freq=assessment_amp;

    //writing data to output
    QVector<double> gui_out;
    gui_out.append(treshold_amp);
    gui_out.append(treshold_freq);
    gui_out.append(assessment_amp);
    gui_out.append(assessment_freq);
    return gui_out;
}

