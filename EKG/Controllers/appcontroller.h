#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>
#include <vector>
#include "Views/airecgmain.h"
#include "Common/ecgdata.h"
#include "Common/ecginfo.h"

#include "ATRIAL_FIBR/src/AtrialFibrApi.h"

typedef QVector<QVector<double>::const_iterator> iters;

class AppController : public QObject
{
    Q_OBJECT

private:
    //SupervisoryModule *supervisor;
    EcgData *entity;
    //results AllData;
    void deep_copy_vect(QVector<unsigned int> &dest, QVector<unsigned int> &src);
    void ResetModules();

public:
    explicit AppController(QObject *parent = 0);
    //void InitializeDependencies();
    //void RunMock();
    void BindView(AirEcgMain *view);

    /*8****useful functions*/
    void ifEcgBaselineExists(void);
    void ifRpeaksExists(void);
    void ifWavesExists(void);
    void ifTWaveExists(void);
    void deleteWaves(void);
    void deleteApnea(void);

    void load12lead_db(VCG_input &input);


signals:
    void patientData(EcgData *info);
    //void sendQRSData(QRSClass qrsSegment, int type);

    void EcgBaseline_done(EcgData *data);
    void AtrialFibr_done (EcgData *data);
    void StInterval_done(EcgData *data);
    void HRV1_done(EcgData *data);
    void RPeaks_done(EcgData *data);
    void Waves_done(EcgData *data);
    void SigEdr_done(EcgData *data);
    void QrsClass_done(EcgData *data);
    void runVcgLoop_done(EcgData *data);
    void SleepApnea_done(EcgData *data);
    void QtDisp_done(EcgData *data);
    void HRT_done(EcgData *data);
    void busy(bool);
public slots:
    void loadData(const QString &directory, const QString &name);
    void switchSignal(int index);
    void switchSignal_SIGEDR(int index);
    void switchEcgBaseline(int type);
    void switchRPeaks(unsigned char type);
    //void sendQRSData(int index, int type);
    void switchTWA(unsigned char type);
    void switchWaves_p_onset(bool check);

    void ecgBase_Kalman1Changed(const QString arg1);
    void ecgBase_Kalman2Changed(const QString arg2);
    void ecgBase_WindowSizeEdit(const QString arg1);
    void ecgButterChanged      (const int set_number);

    void vcg_loop_change(int index);

    void stInterval_detectionWidthChanged(int arg1);
    void stInterval_smoothingWidthChanged(int arg1);
    void stInterval_morphologyChanged(double arg1);
    void stInterval_levelThresholdChanged(double arg1);
    void stInterval_slopeThresholdChanged(double arg1);
    void stInterval_algorithmChanged(int index);

    /*void qrsClustererChanged(ClustererType type);
    void qrsMaxIterationsChanged(int maxIters);
    void qrsParallelExecutionChanged(bool flag);
    void qrsGMinClustersChanged(int minClusters);
    void qrsGMaxClustersChanged(int maxClusters);
    void qrsGMaxKIterations(int maxIters);
    void qrsKClustersNumberChanged(int noClusters);*/

    void run();
    void runEcgBaseline();
    void runAtrialFibr();
    void runStInterval();
    void runHRV1();
    void runRPeaks();
    void runQrsClass();
    void runWaves();
    void runSigEdr();
    void runHRT();
    void runSleepApnea();
    void runVcgLoop();
    void runQtDisp();

};

#endif // APPCONTROLLER_H
