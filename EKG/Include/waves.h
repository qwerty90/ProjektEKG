#ifndef WAVES_H
#define WAVES_H

#include <QtCore/qglobal.h>
#include <QList>
#include <cmath>

#if defined(WAVES_LIBRARY)
#  define WAVESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WAVESSHARED_EXPORT Q_DECL_IMPORT
#endif
class WAVESSHARED_EXPORT Waves
{
    private:
        QList<double> baselinedSignal;
        QList<double> filteredSignal;
        QList<double> SPlane;
        QList<double> SPlaneVelocities;
        QList<double> SPlaneAccelerations;
        QList<double> velocities;
        QList<double> accelerations;
        QList<double> W;
        QList<double> MMD;

        double samplingFrequency;
        double sampleTime;

        QList<unsigned int> rPeaks;

        int ms15;
        int ms20;
        int ms32;
        int ms50;
        int ms200;
        int ms100;
        int ms40;
        int ms16;

        unsigned int DetectQrs_onset(unsigned int rPeakIndex);
        unsigned int DetectQrs_end(unsigned int rPeakIndex);
        unsigned int DetectT_end(unsigned int rPeak, unsigned int Qrs_end);
        void DetectP_wave(unsigned int *P_onset, unsigned int *P_end , unsigned int rPeakIndex, unsigned int Qrs_onset);

        QList<double> Erosion(QList<double> signal, QList<int> B);
        QList<double> Dilation(QList<double> signal, QList<int> B);

        void CalculateVelocities();
        void CalculateAccelerations();
        void CalculateFilteredSignal();
        double Differentiate(QList<double> list,unsigned int pointOnList , unsigned int range);

    public:
        class EcgFrame
        {
            public:
                unsigned int QRS_onset;
                unsigned int QRS_end;
                unsigned int T_end;
                unsigned int P_onset;
                unsigned int P_end;
                EcgFrame();
        };

        Waves(QList<double> baselinedSignal, QList<unsigned int> rPeaks, double samplingFrequency);
        QList<EcgFrame*> DetectWaves();
        ~Waves();
};

#endif // WAVES_H
