#include "appcontroller.h"
#include "Common/ecgdata.h"
#include "Common/ecgentry.h"
//#include "Common/supervisorymodule.h"
#include "ECG_BASELINE/src/butter.h"
#include "ECG_BASELINE/src/kalman.h"

#include <QThread>

AppController::AppController(QObject *parent) : QObject(parent)
{
    //this->InitializeDependencies();
}
/*
void AppController::InitializeDependencies()
{
    //Ecg_BaselineModule *ecg_BaselineModule = new Ecg_BaselineModule();
    //this->supervisor = new SupervisoryModule("ECG_BASELINE", ecg_BaselineModule);
}
*/
void AppController::BindView(AirEcgMain *view)
{
    this->connect(view, SIGNAL(loadEntity(QString,QString)), this, SLOT(loadData(QString,QString)));
    this->connect(view, SIGNAL(switchSignal(int)), this, SLOT(switchSignal(int)));
    this->connect(this, SIGNAL(patientData(EcgData*)), view, SLOT(receivePatientData(EcgData*)));
    this->connect(view, SIGNAL(switchEcgBaseline(int)), this, SLOT(switchEcgBaseline(int)));
    this->connect(view, SIGNAL(switchRPeaks(unsigned char)), this, SLOT(switchRPeaks(unsigned char)));
    this->connect(view, SIGNAL(switchWaves_p_onset(bool)), this, SLOT(switchWaves_p_onset( bool)));
    this->connect(view, SIGNAL(switchTWA(unsigned char)), this, SLOT(switchTWA(unsigned char)));
    this->connect(view, SIGNAL(run()), this, SLOT(run()));
    this->connect(this, SIGNAL(processingResults(EcgData*)), view, SLOT(receiveResults(EcgData*)));
    this->connect(view, SIGNAL(qrsClassChanged(int,int)),this,SLOT(sendQRSData(int,int)));
    this->connect(this, SIGNAL(sendQRSData(QRSClass,int)),view,SLOT(receiveQRSData(QRSClass,int)));
    this->connect(view, SIGNAL(runSingle(QString)), this, SLOT(runSingle(QString)));

    this->connect(view, SIGNAL(runEcgBaseline()),this, SLOT (runEcgBaseline()));//example
    this->connect(this, SIGNAL(EcgBaseline_done(EcgData*)),view, SLOT(drawEcgBaseline(EcgData*)));//example
    this->connect(view, SIGNAL(runAtrialFibr()),this, SLOT (runAtrialFibr()));
    this->connect(this, SIGNAL( AtrialFibr_done(EcgData*)),view,  SLOT(drawAtrialFibr(EcgData*)));

    this->connect(this, SIGNAL(singleProcessingResult(bool, EcgData*)), view, SLOT(receiveSingleProcessingResult(bool, EcgData*)));
    this->connect(view, SIGNAL(qrsClustererChanged(ClustererType)),this,SLOT(qrsClustererChanged(ClustererType)));
    this->connect(view, SIGNAL(qrsGMaxClustersChanged(int)),this,SLOT(qrsGMaxClustersChanged(int)));
    this->connect(view, SIGNAL(qrsGMaxKIterations(int)),this,SLOT(qrsGMaxKIterations(int)));
    this->connect(view, SIGNAL(qrsGMinClustersChanged(int)),this,SLOT(qrsGMinClustersChanged(int)));
    this->connect(view, SIGNAL(qrsKClustersNumberChanged(int)),this,SLOT(qrsKClustersNumberChanged(int)));
    this->connect(view, SIGNAL(qrsMaxIterationsChanged(int)),this,SLOT(qrsMaxIterationsChanged(int)));
    this->connect(view, SIGNAL(qrsParallelExecutionChanged(bool)),this,SLOT(qrsParallelExecutionChanged(bool)));

}

void AppController::loadData(const QString &directory, const QString &name)
{
    EcgEntry *entry = new EcgEntry();
    QString *response = new QString("");
    if(!entry->Open(directory, name, *response))
    {
        return;
    }
    //if(this->entity)
        //this->supervisor->ResetModules();
    this->entity = entry->entity;

    emit patientData(this->entity);
}

void AppController::switchEcgBaseline(int type)
{
    if(this->entity)
        this->entity->settings->EcgBaselineMode = type;
}

void AppController::switchRPeaks(unsigned char type)
{
    if(this->entity)
        this->entity->settings->RPeaksMode = type;
}

void AppController::switchWaves_p_onset(bool check)
{
    //if(this->entity)
      //  this->entity->settings->
}

void AppController::sendQRSData(int index, int type)
{
    if (!this->entity || !this->entity->classes || !this->entity->waves || !this->entity->ecg_baselined || index < 0)
        return;
    QRSClass qrsSegment;

    if (type == 1)
    {
        if (this->entity->classes->count() > index)
        {
            qrsSegment = this->entity->classes->at(index);
        }
        else
            return;
    }
    else
    {
        if (this->entity->waves->count() <= index)
            return;
        int begin = this->entity->waves->at(index)->QRS_onset;
        int end = this->entity->waves->at(index)->QRS_end + 1;

        qrsSegment.representative = new QVector<double>();
        for(int i = begin; i < end; i++)
            qrsSegment.representative->append(this->entity->ecg_baselined->at(i));
    }
    emit sendQRSData(qrsSegment,type);
}

void AppController::switchTWA(unsigned char type)
{
    if(this->entity)
        this->entity->TWA_mode = type;
}

void AppController::qrsClustererChanged(ClustererType type)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.clusterer = type;
}

void AppController::qrsMaxIterationsChanged(int maxIters)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.maxIterations = maxIters;
}

void AppController::qrsParallelExecutionChanged(bool flag)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.parallelExecution = flag;
}

void AppController::qrsGMinClustersChanged(int minClusters)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.minClusterNo = minClusters;
}

void AppController::qrsGMaxClustersChanged(int maxClusters)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.maxClusterNo = maxClusters;
}

void AppController::qrsGMaxKIterations(int maxIters)
{
    if(this->entity)
        this->entity->settings->qrsClassSettings.insideIterations = maxIters;
}

void AppController::qrsKClustersNumberChanged(int noClusters)
{
    if(this->entity)
    {
        this->entity->settings->qrsClassSettings.minClusterNo = noClusters;
        this->entity->settings->qrsClassSettings.maxClusterNo = noClusters;
    }
}

void AppController::run()
{
    /*
    if(this->entity)
    {
        this->supervisor->entity = this->entity;
        connect(this->supervisor, SIGNAL(Finished()), this, SLOT(onThreadFinished()));
        this->supervisor->start();
    }*/
}

void AppController::onThreadFinished()
{
    emit this->processingResults(this->entity);
}

void AppController::runSingle(QString hash)
{



    /*
    if(this->entity)
    {
        bool processed = this->supervisor->RunSingle(this->entity, hash);
        emit singleProcessingResult(processed, this->entity);
    }
    */
}

void AppController::switchSignal(int index)
{

    this->entity->settings->signalIndex = index;
    /*
    this->supervisor->ResetModules();
    */
}
void AppController::runEcgBaseline()
{
    QLOG_INFO() <<"ecg started";

    QVector<double> test;
    test << 0.5 << 0.5 << 0.5;
    KalmanFilter kalman;
    kalman.processKalman(test);

    QLOG_INFO() << "rysowanie ecg->emit";
    emit EcgBaseline_done(this->entity);
}

void AppController::deep_copy_list(QList<int> *dest, QList<int> *src)
{
    //QList<int>::Iterator iter_dest = dest;
    QList<int>::Iterator iter_src  = src->begin();

    while (iter_src != src->end())
    {
        dest->append(*iter_src);
        iter_src++;
    }

}

void AppController::runAtrialFibr()
{
    QLOG_INFO() << "Start AtrialFibr";

    //wrzucenie przykladowych danych
    this->entity->Rpeaks     = new QVector<QVector<double>::const_iterator>
                              ({this->entity->ecg_baselined->begin() + 20,
                                this->entity->ecg_baselined->begin() + 80});

    this->entity->PWaveStart = new QVector<QVector<double>::const_iterator>
                              ({this->entity->ecg_baselined->begin() + 10,
                                this->entity->ecg_baselined->begin() + 70});
    //this->entity->PWaveStart = new QVector<QVector<double>::const_iterator>;

    //const std::vector<double> signal((this->entity->ecg_baselined)->toStdVector());
    //const std::vector<std::vector<double>::const_iterator> RPeaks((this->entity->Rpeaks)->toStdVector());
    //const std::vector<std::vector<double>::const_iterator> pWaveStarts((this->entity->PWaveStart)->toStdVector());


    /*this->entity->Rpeaks->append(this->entity->ecg_baselined->begin() + 20);
    this->entity->Rpeaks->append(this->entity->ecg_baselined->begin() + 80);
    this->entity->PWaveStart->append(this->entity->ecg_baselined->begin() + 10);
    this->entity->PWaveStart->append(this->entity->ecg_baselined->begin() + 70);*/

    AtrialFibrApi obiekt(*(this->entity->ecg_baselined),
                         *(this->entity->Rpeaks) ,
                         *(this->entity->PWaveStart) )   ;

    this->entity->PWaveOccurenceRatio= obiekt.GetPWaveOccurenceRatio();
    this->entity->RRIntDivergence    = obiekt.GetRRIntDivergence();
    this->entity->RRIntEntropy       = obiekt.GetRRIntEntropy();
    this->entity->AtrialFibr         = obiekt.isAtrialFibr();

    emit AtrialFibr_done(this->entity);
    QLOG_INFO() << "AtrialFibr done";

}
