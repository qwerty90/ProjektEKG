#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>
#include <vector>
//#include "Common/supervisorymodule.h"
#include "Views/airecgmain.h"
#include "Common/ecgdata.h"
#include "Common/ecginfo.h"

#include "ATRIAL_FIBR/src/AtrialFibrApi.h"

class AppController : public QObject
{
    Q_OBJECT

private:
    //SupervisoryModule *supervisor;
    EcgData *entity;
    //results AllData;
    void deep_copy_list(QList<int> *dest, QList<int> *src);
public:
    explicit AppController(QObject *parent = 0);
    //void InitializeDependencies();
    //void RunMock();
    void BindView(AirEcgMain *view);
signals:
    void patientData(EcgData *info);
    void sendQRSData(QRSClass qrsSegment, int type);
    void singleProcessingResult(bool succeeded, EcgData *data);
    void processingResults(EcgData *data);

    void EcgBaseline_done(EcgData *data);
    void AtrialFibr_done (EcgData *data);
public slots:
    void loadData(const QString &directory, const QString &name);
    void switchSignal(int index);
    void switchEcgBaseline(int type);
    void switchRPeaks(unsigned char type);
    void sendQRSData(int index, int type);
    void switchTWA(unsigned char type);
    void switchWaves_p_onset(bool check);

    void qrsClustererChanged(ClustererType type);
    void qrsMaxIterationsChanged(int maxIters);
    void qrsParallelExecutionChanged(bool flag);
    void qrsGMinClustersChanged(int minClusters);
    void qrsGMaxClustersChanged(int maxClusters);
    void qrsGMaxKIterations(int maxIters);
    void qrsKClustersNumberChanged(int noClusters);

    void run();
    void runSingle(QString hash);
    void runEcgBaseline();//example
    void runAtrialFibr();
    void onThreadFinished();
};

#endif // APPCONTROLLER_H
