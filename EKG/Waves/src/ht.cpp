#include <iostream>
#include <fstream>
#include <iterator>
#include <QVector>
#include <cmath>
#include <cstdlib>

//#define LMAX 200000
#define LFILT 128
#define pi 3.14159265358979323846

using namespace std;

void hilbert_transform(QVector<double>::iterator signal_start, QVector<double>::iterator signal_end, QVector<double>::iterator hilbert_start, QVector<double>::iterator hilbert_end)
{
    int i, npt, lfilt;
    double *xh=new double[signal_end-signal_start];
    double hilb[LFILT+1], pi2,yt;

    pi2 = 2*pi;
    lfilt = LFILT;

    for (i=1; i<=lfilt; i++){
        hilb[i]=1/((i-lfilt/2)-0.5)/pi;
    }

    npt=signal_end-signal_start;


 //zastąpić wyjątkiem!
    if (npt < lfilt) {
        fprintf(stderr, "insufficient data\n");
        exit(2);
    }


    //wyznaczanie convol
        for (int l=1; l<=npt-lfilt+1; l++) {
            yt = 0.0;
            for (int i=1; i<=lfilt; i++)
                yt = yt+(*(signal_start+(l+i-2)))*hilb[lfilt+1-i];
            xh[l] = yt;
        }

        /* shifting lfilt/1+1/2 points */
        for (int i=1; i<=npt-lfilt; i++) {
            xh[i] = 0.5*(xh[i]+xh[i+1]);
        }
        for (int i=npt-lfilt; i>=1; i--)
            xh[i+lfilt/2]=xh[i];

        /* writing zeros */
        for (int i=1; i<=lfilt/2; i++) {
            xh[i] = 0.0;
            xh[npt+1-i] = 0.0;
        }
        i=1;
        QVector<double>::iterator it;
        for(it=hilbert_start; it!=hilbert_end; it++) //zamienić xh na wektor zamiast przepisywać.
        {
            *it=xh[i];
            i++;
        }
        delete [] xh;
}
