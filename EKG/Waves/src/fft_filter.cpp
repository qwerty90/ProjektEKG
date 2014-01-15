#include <iostream>
#include <iterator>
#include <QVector>
#include <fstream>
#include <cmath>
#include "kiss_fft.h"
#include "kiss_fftr.h"


using namespace std;

kiss_fft_cpx* copycpx(QVector<double>::iterator ecg_start, int nframe)
{
    int i;
    kiss_fft_cpx *mat2;
    mat2=(kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*nframe);
        kiss_fft_scalar zero;
        memset(&zero,0,sizeof(zero) );
    for(i=0; i<nframe ; i++)
    {
        mat2[i].r = *(ecg_start+i);
        mat2[i].i = zero;
    }
    return mat2;
}

void fft_filter(QVector<double>::iterator ecg_start, QVector<double>::iterator ecg_end, QVector<double>::iterator signal_start, QVector<double>::iterator signal_end, float fs, float low_freq_lim, float hi_freq_lim)
{
    int size = ecg_end-ecg_start;
    int isinverse = 1;
    kiss_fft_scalar zero;
    kiss_fft_cpx *out_cpx=new kiss_fft_cpx[size];
    kiss_fft_cpx *out=new kiss_fft_cpx[size];
    kiss_fft_cpx *cpx_buf;

    kiss_fftr_cfg fft = kiss_fftr_alloc(size*2 ,0 ,0,0);
    kiss_fftr_cfg ifft = kiss_fftr_alloc(size*2,isinverse,0,0);

    cpx_buf = copycpx(ecg_start,size);
    kiss_fftr(fft,(kiss_fft_scalar*)cpx_buf, out_cpx);
    for (int i=0; i<size; i++)
    {
        if(i<floor(low_freq_lim/(fs/size))-1 || (i>hi_freq_lim/(fs/size) && i<size-hi_freq_lim/(fs/size)) || i>size-low_freq_lim/(fs/size))
        {
            out_cpx[i].r=zero;
            out_cpx[i].i=zero;
        }
    }

    kiss_fftri(ifft,out_cpx,(kiss_fft_scalar*)out );

    for(int i=0;i<size;i++)
    {
         *(signal_start+i)=out[i].r/(size*2);
    }
    delete [] out_cpx;
    delete [] out;
    kiss_fft_cleanup();
    free(fft);
    free(ifft);
}
