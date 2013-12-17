#ifndef HRV2_H
#define HRV2_H

#include <QtCore/qglobal.h>
#include "../Common/processingmodule.h"

#if defined(HRV2_LIBRARY)
#  define HRV2SHARED_EXPORT Q_DECL_EXPORT
#else
#  define HRV2SHARED_EXPORT Q_DECL_IMPORT
#endif

class HRV2SHARED_EXPORT HRV2
{
public:

    // input
    QList<unsigned int> rPeaks;
    double sampleFreq;

    // output
    QList<unsigned int> histogram_x, poincare_x;
    QList<int> histogram_y, poincare_y;
    double triangularIndex, TINN, SD1, SD2;

    // main method
    void Process();

private:
    int DopasujWielokrotnoscSredniej(int srednia, double wartosc);
};

#endif // HRV2_H
