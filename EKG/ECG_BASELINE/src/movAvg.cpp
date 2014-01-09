#include "movAvg.h"
#include "QPointF"
#include "math.h"

#define PI 3.14159265

QVector<double> processMovAvg(const QVector<double> &signal, int samplingFrequencyHz, double averagingTimeS)
{
    int windowSize = ceil(samplingFrequencyHz * averagingTimeS);
    return processMovAvg(signal, windowSize);
}

QVector<double> processMovAvg(const QVector<double>& signal, int windowSize)
{
    QVector<double> xx;
    int signalSize = signal.size();
    xx.reserve(signalSize);
    xx.append(signal[0]);
    //obliczenie sredniej kroczacej sygnalu o dlugosi okna windowSize
    for(int i = 1;i < signalSize; i++){
        if (i < windowSize){
            double result = (xx[i-1] * i + signal[i]) / (i+1);
            xx.append( result );
        }
        else{
            double result = xx[i-1] + (signal[i] - signal[i-windowSize]) / windowSize;
            xx.append( result );
        }
    }
    //odjecie skladowej stalej od sygnalu
    for(int i = 0;i < signalSize; i++){
        xx[i] = signal[i]-xx[i];
    }

    return xx;
}

QVector<QPointF> movAvgMagPlot(int samplingFrequencyHz, double averagingTimeS)
{
    int windowSize = ceil(samplingFrequencyHz/averagingTimeS);

    QVector<QPointF> plotData(700);
    plotData[0] = QPointF(0, 1);
    for(int i = 1; i < plotData.size(); i++) {
        double x = double(i) / (plotData.size() + 300);
        double y = sin(PI * x * windowSize) / (windowSize * sin(PI * x));
        plotData[i - 1] = QPointF(x, fabs(y));
    }

    return plotData;
}
