#ifndef HRV_DFA_H
#define HRV_DFA_H

#include <QtCore/qglobal.h>
//#include "../../../Common/ecgdata.h"
#include "../Common/processingmodule.h"

#if defined(HRV_DFA_LIBRARY)
#  define HRV_DFASHARED_EXPORT Q_DECL_EXPORT
#else
#  define HRV_DFASHARED_EXPORT Q_DECL_IMPORT
#endif

class HRV_DFASHARED_EXPORT HRV_DFA
{
public:

    // input
    QList<unsigned int> r_peaks;
    double sampleFreq;

    //HRV_DFA();
    void Process();

    QList<double> trend_y, trend_z;
    QList<double> trend_x, trend_v;
    double  alfa, wsp_a, wsp_b;
    int window_min, window_max, window_plot, boxes;

};

#endif // HRV_DFA_H
