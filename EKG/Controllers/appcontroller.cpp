#include "appcontroller.h"
#include "Common/ecgdata.h"
#include "Common/ecgentry.h"
#include "ECG_BASELINE/src/butter.h"
#include "ECG_BASELINE/src/kalman.h"
#include "ECG_BASELINE/src/movAvg.h"
#include "ECG_BASELINE/src/sgolay.h"
#include "ST_INTERVAL/ecgstanalyzer.h"
#include "QRS_CLASS/qrsclass.h"
#include "HRV1/HRV1MainModule.h"
#include "R_PEAKS/src/r_peaksmodule.h"
#include "Waves/src/waves.h"
#include "SIG_EDR/sig_edr.h"
#include "HRT/HRTmodule.h"
#include "SLEEP_APNEA/src/sleep_apnea.h"
#include "VCG_T_LOOP/vcg_t_loop.h"
#include <fstream>
#include <iostream>

#include <QThread>

AppController::AppController(QObject *parent) : QObject(parent)
{
    this->entity = new EcgData;
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
    this->connect(view, SIGNAL(switchSignal_SIGEDR(int)), this, SLOT(switchSignal_SIGEDR(int)));
    this->connect(this, SIGNAL(patientData(EcgData*)), view, SLOT(receivePatientData(EcgData*)));
    this->connect(view, SIGNAL(switchEcgBaseline(int)), this, SLOT(switchEcgBaseline(int)));
    this->connect(view, SIGNAL(switchRPeaks(unsigned char)), this, SLOT(switchRPeaks(unsigned char)));
    this->connect(view, SIGNAL(switchWaves_p_onset(bool)), this, SLOT(switchWaves_p_onset( bool)));
   // this->connect(view, SIGNAL(switchTWA(unsigned char)), this, SLOT(switchTWA(unsigned char)));

    this->connect(view, SIGNAL(qrsClassChanged(int,int)),this,SLOT(sendQRSData(int,int)));
    this->connect(this, SIGNAL(sendQRSData(QRSClass,int)),view,SLOT(receiveQRSData(QRSClass,int)));

    this->connect(view, SIGNAL(ecgBase_WindowSizeChanged(QString)),this,SLOT(ecgBase_WindowSizeEdit(QString)));
    this->connect(view, SIGNAL(ecgBase_ButterworthCoeffSetChanged(int)), this, SLOT(ecgButterChanged(int)));

    this->connect(view, SIGNAL(stInterval_detectionWidthChanged(int)),this,SLOT(stInterval_detectionWidthChanged(int)));
    this->connect(view, SIGNAL(stInterval_smoothingWidthChanged(int)),this,SLOT(stInterval_smoothingWidthChanged(int)));
    this->connect(view, SIGNAL(stInterval_morphologyChanged(double)),this,SLOT(stInterval_morphologyChanged(double)));
    this->connect(view, SIGNAL(stInterval_levelThresholdChanged(double)),this,SLOT(stInterval_levelThresholdChanged(double)));
    this->connect(view, SIGNAL(stInterval_slopeThresholdChanged(double)),this,SLOT(stInterval_slopeThresholdChanged(double)));
    this->connect(view, SIGNAL(stInterval_algorithmChanged(int)),this,SLOT(stInterval_algorithmChanged(int)));

    this->connect(view, SIGNAL(runEcgBaseline()),this, SLOT (runEcgBaseline()));//example

    this->connect(view, SIGNAL(runAtrialFibr()) ,this, SLOT (runAtrialFibr()));
    this->connect(view, SIGNAL(runStInterval()) ,this, SLOT (runStInterval()));
    this->connect(view, SIGNAL(runHRV1())       ,this, SLOT (runHRV1()));
    this->connect(view, SIGNAL(runRPeaks())     ,this, SLOT (runRPeaks()));
    this->connect(view, SIGNAL(runWaves())      ,this, SLOT (runWaves()));
    this->connect(view, SIGNAL(runSigEdr())     ,this, SLOT (runSigEdr()));
    this->connect(view, SIGNAL(runVcgLoop())    ,this, SLOT (runVcgLoop()));
    this->connect(view, SIGNAL(runSleepApnea()) ,this, SLOT (runSleepApnea()));
    this->connect(view, SIGNAL(runQtDisp())     ,this, SLOT (runQtDisp()));
    this->connect(view, SIGNAL(runHRT())        ,this, SLOT (runHRT()));
    this->connect(view, SIGNAL(run())           ,this, SLOT (run()));
    this->connect(view, SIGNAL(runQrsClass())   ,this, SLOT (runQrsClass()));

    this->connect(this, SIGNAL(EcgBaseline_done(EcgData*)),view, SLOT(drawEcgBaseline(EcgData*)));//example
    this->connect(this, SIGNAL( AtrialFibr_done(EcgData*)),view, SLOT(drawAtrialFibr(EcgData*)));
    this->connect(this, SIGNAL(StInterval_done(EcgData*)) ,view, SLOT(drawStInterval(EcgData*)));
    this->connect(this, SIGNAL(HRV1_done(EcgData*))       ,view, SLOT(drawHrv1(EcgData*)))      ;
    this->connect(this, SIGNAL(RPeaks_done(EcgData*))     ,view, SLOT(drawRPeaks(EcgData*)))    ;
    this->connect(this, SIGNAL(Waves_done(EcgData*))      ,view, SLOT(drawWaves(EcgData*)))     ;
    this->connect(this, SIGNAL(SigEdr_done(EcgData*))     ,view, SLOT(drawSigEdr(EcgData*)))    ;
    this->connect(this, SIGNAL(QrsClass_done(EcgData*))   ,view, SLOT(drawQrsClass(EcgData*)))  ;
    this->connect(this, SIGNAL(runVcgLoop_done(EcgData*)) ,view, SLOT(drawVcgLoop(EcgData*)))   ;
    this->connect(this, SIGNAL(SleepApnea_done(EcgData*)) ,view, SLOT(drawSleep_Apnea(EcgData*)));
    this->connect(this, SIGNAL(QtDisp_done(EcgData*))     ,view, SLOT(drawQtDisp(EcgData*)))    ;
    this->connect(this, SIGNAL(HRT_done(EcgData*))        ,view, SLOT(drawHrt(EcgData*)))       ;
    this->connect(this, SIGNAL(busy(bool))                ,view, SLOT(busy(bool)))              ;
    this->connect(this, SIGNAL(QrsClass_done(EcgData*))   ,view, SLOT(drawQrsClass(EcgData*)))  ;

    this->connect(view, SIGNAL(qrsClustererChanged(ClustererType)),this,SLOT(qrsClustererChanged(ClustererType)));
    this->connect(view, SIGNAL(qrsGMaxClustersChanged(int)),this,SLOT(qrsGMaxClustersChanged(int)));
    this->connect(view, SIGNAL(qrsGMaxKIterations(int)),this,SLOT(qrsGMaxKIterations(int)));
    this->connect(view, SIGNAL(qrsGMinClustersChanged(int)),this,SLOT(qrsGMinClustersChanged(int)));
    this->connect(view, SIGNAL(qrsKClustersNumberChanged(int)),this,SLOT(qrsKClustersNumberChanged(int)));
    this->connect(view, SIGNAL(qrsMaxIterationsChanged(int)),this,SLOT(qrsMaxIterationsChanged(int)));
    this->connect(view, SIGNAL(qrsParallelExecutionChanged(bool)),this,SLOT(qrsParallelExecutionChanged(bool)));

    this->connect(view, SIGNAL(vcg_loop_change(int)),this,SLOT(vcg_loop_change(int)));

}

void AppController::loadData(const QString &directory, const QString &name)
{
    EcgEntry *entry = new EcgEntry;
    QString *response = new QString("");
    if(!entry->Open(directory, name, *response))
    {
        return;
    }
    if(this->entity)
        this->ResetModules();
    this->entity = entry->entity;

    this->entity->butter_coeffs = new QVector<ButterCoefficients>(predefinedButterCoefficientSets());
        //const QVector<ButterCoefficients> coeff = predefinedButterCoefficientSets();

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

    if (!this->entity || !this->entity->classes || !this->entity->Waves->PWaveStart|| !this->entity->ecg_baselined || index < 0)
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
        if (this->entity->Waves->Count <= index)
            return;
        int begin = (this->entity->Waves->QRS_onset->at(index)-this->entity->ecg_baselined->begin());
        int end = (this->entity->Waves->QRS_end->at(index)-this->entity->ecg_baselined->begin());

//ecgFrames.PWaveStart->at(i)-ecgSignal.begin()
        qrsSegment.representative = new QVector<double>();
        for(int i = begin; i < end; i++)
            qrsSegment.representative->append(this->entity->ecg_baselined->at(i));
    }
    emit sendQRSData(qrsSegment,type);
}

void AppController::switchTWA(unsigned char type)
{
    QLOG_FATAL()<<"MVC/ Unused slot.";
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


void AppController::switchSignal(int index)
{

    this->entity->settings->signalIndex = index;
    this->ResetModules();
}
void AppController::switchSignal_SIGEDR(int index)
{
    this->entity->settings->SigEdr_lead = index;
}
void AppController::vcg_loop_change(int index)
{
    int ecg_index = 1 ;//tutaj jakies pole z ecgdata
    if(ecg_index>0)
    {
        if(index==1) ecg_index++;
        if(index==0) ecg_index--;
    }
}


void AppController::deep_copy_vect(QVector<unsigned int> &dest, QVector<unsigned int> &src)
{
    dest.resize(src.size());

    for (int i=0 ; i<src.size() ; i++)
        dest[i]=src.at(i);
}

void AppController::ResetModules()
{
    QLOG_INFO() << "Reset procedure started:";
    if (this->entity->ecg_baselined)
    {
        this->entity->ecg_baselined->clear();
        this->entity->ecg_baselined=NULL;
        QLOG_INFO() << "MVC/ Baselined signal removed.";
    }

    if (this->entity->Rpeaks)
    {
        this->entity->Rpeaks->clear();
        this->entity->Rpeaks=NULL;
        QLOG_INFO() << "MVC/ Rpeaks removed.";
    }
    if (!this->entity->Rpeaks_uint.isEmpty())
    {
        this->entity->Rpeaks_uint.clear();
        QLOG_INFO() << "MVC/ Rpeaks uint removed.";
    }
    if (this->entity->hrt_tachogram!=NULL)
    {
        this->entity->hrt_tachogram->clear();
        this->entity->hrt_tachogram=NULL;
        QLOG_INFO() <<"MVC/ HRT removed.";
    }
    if (this->entity->TWaveStart!=NULL)
    {
        this->entity->TWaveStart->clear();
        this->entity->TWaveStart=NULL;
    }

    deleteHRV1();
    deleteWaves();
    deleteApnea();

    QLOG_INFO() << "All removed.";
}

void AppController::runEcgBaseline()
{
    QLOG_INFO() <<"Ecg baseline started.";
    if (this->entity->primary==NULL || this->entity->secondary==NULL)
    {
        QLOG_FATAL() << "No data loaded";
        return;
    }

    const QVector<ButterCoefficients> coeff = predefinedButterCoefficientSets();

    if (this->entity->ecg_baselined!=NULL)
    {
        this->entity->ecg_baselined->clear();
    }

    switch (this->entity->settings->EcgBaselineMode)
    {
    case 0: //butterworth
        QLOG_INFO() << "BASELINE/ Using butterworth filter with coefficiets "
                       <<((*this->entity->butter_coeffs)[this->entity->settings->coeff_set]).name();
        this->entity->ecg_baselined =
                new QVector<double>(processButter(
                                        *(this->entity->GetCurrentSignal()),
                                        ((*this->entity->butter_coeffs)[this->entity->settings->coeff_set])));
        break;
    case 1:
        QLOG_INFO() << "BASELINE/ Using moving average filter.";
//        if(this->entity->settings->averaging_time!=0)
//        {
//            QLOG_INFO() << "BASELINE/ Using moving average filter with averaging time = "
//                        << QString::number(this->entity->settings->averaging_time) << " .";
//            this->entity->ecg_baselined =
//                    new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),
//                                        (int)(this->entity->info->frequencyValue),
//                                         this->entity->settings->averaging_time));
//            this->entity->characteristics =
//                    new QVector<QPointF>(movAvgMagPlot((int)(this->entity->info->frequencyValue),
//                                         this->entity->settings->averaging_time));
//        }
        QLOG_INFO() << "BASELINE/ Using moving average filter with window width = "
                    << QString::number(this->entity->settings->avgWindowSize) << " .";
        this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),
                                                                        this->entity->settings->avgWindowSize));
        break;

    case 2: //savitzky-golay
        QLOG_INFO() << "BASELINE/ Using Savitzky-Golay filter.";
        this->entity->ecg_baselined = new QVector<double>(processSGolay(*(this->entity->GetCurrentSignal())));
        break;
    case 3:  //kalman
        QLOG_INFO() << "BASELINE/ Using kalman filter.";

        this->entity->ecg_baselined = new QVector<double>(processKalman(*(this->entity->GetCurrentSignal()),
                                                                        entity->info->frequencyValue));
        break;
    default:
        QLOG_INFO() << "BASELINE/ Using default filter.";
        this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),
                                                                        entity->settings->avgWindowSize));
        break;
    }            

    QLOG_INFO() << "Ecg baseline done.";

    emit EcgBaseline_done(this->entity);
    emit busy(false);

         //runVcgLoop();//  <- nie odpalajcie tego!

}

void AppController::runHRV1()
{
    QLOG_INFO() << "HRV1 started.";
    ifRpeaksExists();

    if ( (this->entity->fft_x != NULL)
         &(this->entity->fft_y != NULL)
         &(this->entity->RR_x != NULL)
         &(this->entity->RR_y != NULL) )
    {
        QLOG_INFO() << "MVC/ HRV1 already exists.";
        return;
    }


    QVector<int> *wektor = new QVector<int>(this->entity->Rpeaks_uint.size());

    for (int i=0 ; i<wektor->size();i++)
    {
        (*wektor)[i] = (int)this->entity->Rpeaks_uint.at(i);
    }

    HRV1MainModule obiekt;
    obiekt.prepare(wektor,(int)this->entity->info->frequencyValue);
    HRV1BundleStatistical results = obiekt.evaluateStatistical();
    this->entity->Mean = results.RRMean;
    this->entity->SDNN = results.SDNN;
    this->entity->RMSSD= results.RMSSD;
    this->entity->RR50 = results.NN50;    //czy to jest to??
    this->entity->RR50Ratio=results.pNN50;//czy to jest to??
    this->entity->SDANN= results.SDANN;
    this->entity->SDANNindex=results.SDANNindex;
    this->entity->SDSD = results.SDSD;
    QLOG_INFO() << "HRV1 statistical done.";

    HRV1BundleFrequency results_freq = obiekt.evaluateFrequency();
    this->entity->fft_x = new QVector<double>(*(results_freq.xData));
    this->entity->fft_y = new QVector<double>(*(results_freq.yData));
    this->entity->TP = results_freq.TP;
    this->entity->HF = results_freq.HF;
    this->entity->LF = results_freq.LF;
    this->entity->ULF = results_freq.ULF;
    this->entity->VLF = results_freq.VLF;
    this->entity->LFHF = results_freq.LFHF;

    this->entity->RR_x = new QVector<double>(*(results_freq.rrXData));
    this->entity->RR_y = new QVector<double>(*(results_freq.rrYData));

    QLOG_INFO() << "HRV1 frequency done.";

    emit this->HRV1_done(this->entity);
    results_freq.xData->clear();
    results_freq.yData->clear();
    results_freq.rrXData->clear();
    results_freq.rrYData->clear();
    QLOG_TRACE() << "HRV1 statistical drawn.";
}

void AppController::runAtrialFibr()
{
    QLOG_INFO() << "Start AtrialFibr";

    ifWavesExists();

    if (this->entity->Waves->PWaveStart== NULL )
    {
        QLOG_FATAL() << "Brak PWaveStart dla modulu AtrialFibr";
        return;
    }

    QString sig_name;
    this->entity->settings->signalIndex?sig_name=this->entity->info->secondaryName
                                        :sig_name=this->entity->info->primaryName;

    AtrialFibrApi obiekt(*(this->entity->ecg_baselined),
                         *(this->entity->Rpeaks) ,
                         *(this->entity->Waves->PWaveStart),
                         sig_name)   ;

    this->entity->PWaveOccurenceRatio= obiekt.GetPWaveAbsenceRatio();
    this->entity->RRIntDivergence    = obiekt.GetRRIntDivergence();
    this->entity->RRIntEntropy       = obiekt.GetRRIntEntropy();
    this->entity->AtrialFibr         = obiekt.isAtrialFibr();

    QLOG_TRACE() << "Atrial_FIBR/ calculated parameters: \n"
                << "Atrial_FIBR/ PWaveOccurenceRatio: " << QString::number(this->entity->PWaveOccurenceRatio) <<"\n"
                << "Atrial_FIBR/ RRIntDivergence: "     << QString::number(this->entity->RRIntDivergence) <<"\n"
                << "Atrial_FIBR/ RRIntEntropy: "        << QString::number(this->entity->RRIntEntropy);

    QLOG_INFO() << "AtrialFibr done";
    emit AtrialFibr_done(this->entity);//linia 37


}
void AppController::runRPeaks()
{
    QLOG_INFO() << "RPeaks stared." ;

    ifEcgBaselineExists();

    if (this->entity->Rpeaks!=NULL)
        this->entity->Rpeaks->clear();
    if (!(this->entity->Rpeaks_uint.isEmpty()))
        this->entity->Rpeaks_uint.clear();

    R_peaksModule obiekt(*(this->entity->ecg_baselined), this->entity->info->frequencyValue);
    switch (this->entity->settings->RPeaksMode)
    {
    case 1:
        QLOG_INFO() << "RPeaks/ using Hilbert";
        obiekt.hilbert();
        break;
    case 2:
        QLOG_INFO() << "RPeaks/ using PanTompkins";
        obiekt.panTompkins();
        break;
    case 3:
        QLOG_INFO() << "RPeaks/ using wavelet";
        obiekt.wavelet();
        break;
    default:
        QLOG_INFO() << "RPeaks/ using default (PanTompkins)";
        obiekt.panTompkins();
    }
    //this->entity->Rpeaks = new iters (obiekt.getPeaksIter());
    this->entity->Rpeaks_uint = obiekt.getPeaksIndex();
    iters tmp_it;
    for(int i=0 ; i<this->entity->Rpeaks_uint.size();i++)
        tmp_it.append(&((*this->entity->ecg_baselined)[ this->entity->Rpeaks_uint.at(i) ]));

    this->entity->Rpeaks = new iters (tmp_it);

    emit this->RPeaks_done(this->entity);
    QLOG_INFO() << "RPeaks done." ;

    /*
    QLOG_INFO() << "MVC/ iters range : \n"
                <<QString::number((int)this->entity->ecg_baselined->begin())<<"    "
                <<QString::number((int)this->entity->ecg_baselined->end())  ;
    for (int i=0 ; i<this->entity->Rpeaks->size();i++)
        QLOG_TRACE()<<"Rpeak "<<i<<" "<<QString::number((int)this->entity->Rpeaks->at(i));
        */
}

void AppController::runStInterval()
{
    QLOG_INFO() << "Start StInterval";

    EcgStAnalyzer analyzer;
    if (this->entity->settings->quadratic)
        analyzer.setAlgorithm(EcgStAnalyzer::QUADRATIC);
    else
        analyzer.setAlgorithm(EcgStAnalyzer::LINEAR);

    analyzer.setDetectionSize(this->entity->settings->detect_window);
    analyzer.setSmoothSize(this->entity->settings->smooth_window);
    analyzer.setLevelThreshold(this->entity->settings->level_tresh);
    analyzer.setSlopeThreshold(this->entity->settings->slope_tresh);
    analyzer.setMorphologyCoeff(this->entity->settings->morph_coeff);

    ifTWaveExists();
    if (this->entity->Waves->T_end == NULL)
    {
        QLOG_FATAL() << "ST_INTERVAL/ no Twave_end for me!";
        return;
    }

    bool res = analyzer.analyze(*(this->entity->ecg_baselined),
                                *(this->entity->Rpeaks),
                                *(this->entity->Waves->QRS_end),
                                *(this->entity->Waves->T_end),
                                static_cast<double>(this->entity->info->frequencyValue));
    if (!res)
    {
        EcgStAnalyzer::ErrorType error = analyzer.getLastError();
        QLOG_FATAL() << "ST_INTERVAL analyzer error: " << error;
    }

    this->entity->STintervals = new QList<EcgStDescriptor>(analyzer.getResult());

    QLOG_INFO() << this->entity->STintervals->size() << "StIntervals calculated.";
    //wyznaczanie TwaveStart
    if (this->entity->TWaveStart!=NULL)
    {
        this->entity->TWaveStart->clear();
        this->entity->TWaveStart=NULL;
    }
    this->entity->TWaveStart = new iters;
    QList<EcgStDescriptor>::iterator it = this->entity->STintervals->begin();
    while(it!=this->entity->STintervals->end())
    {
        QLOG_TRACE() << *((*it).STEnd);
        this->entity->TWaveStart->append((*it).STEnd);
        it++;
    }
    QLOG_INFO() << "StInterval calculated"<<this->entity->TWaveStart->size()<<"ST interval ends.";

    emit StInterval_done(this->entity);
    QLOG_INFO() << "StInterval done";
}

void AppController::runQrsClass()
{
    QLOG_INFO() << "Start QrsClass";

    ifWavesExists();

    QRSClassModule QrsClassifier;
    QRSClassSettings QrsClassifierSettings;
    //ustaw jakieś settingsy
    QrsClassifierSettings.maxIterations = this->entity->settings->QRSClass_maxIterations;
    QrsClassifierSettings.clusterer = static_cast<ClustererType>(this->entity->settings->QRSClass_clusterer);
    QrsClassifier.setSettings(QrsClassifierSettings);

    QrsClassifier.setWaves(this->entity->Waves->QRS_onset, this->entity->Waves->QRS_end);
    QrsClassifier.setEGCBaseline(this->entity->ecg_baselined);

    if (!QrsClassifier.process())
    {
        qDebug() << QrsClassifier.getErrorMessage();
    }
    else
    {
        QVector<QRSClass>* classes = QrsClassifier.getClasses();
        this->entity->classes = classes;        
    }

    emit QrsClass_done(this->entity);
    QLOG_INFO() << "QrsClass done";
}

void AppController::runVcgLoop()
{
    QLOG_INFO() << "Start VcgLoop (not ready yet).";

    this->entity->VCG_raw->I  = new QVector<double>;
    this->entity->VCG_raw->II = new QVector<double>;
    this->entity->VCG_raw->V1 = new QVector<double>;
    this->entity->VCG_raw->V2 = new QVector<double>;
    this->entity->VCG_raw->V3 = new QVector<double>;
    this->entity->VCG_raw->V4 = new QVector<double>;
    this->entity->VCG_raw->V5 = new QVector<double>;
    this->entity->VCG_raw->V6 = new QVector<double>;

    load12lead_db(*(this->entity->VCG_raw));

    QVector<double> *tmp;

    if(this->entity->settings->signalIndex==0)
    {
        tmp = new QVector<double>(*this->entity->primary);
        this->entity->primary = new QVector<double>(*this->entity->VCG_raw->V1);
    }
    else
    {
        tmp = new QVector<double>(*this->entity->secondary);
        this->entity->secondary = new QVector<double>(*this->entity->VCG_raw->V1);
    }

    runStInterval();


    QLOG_TRACE() << "Samples for vcg:"
                 <<this->entity->VCG_raw->I->size()<<"\n"
                   <<this->entity->VCG_raw->II->size()<<"\n"
                     <<this->entity->VCG_raw->V1->size()<<"\n"
                       <<this->entity->VCG_raw->V2->size()<<"\n"
                         <<this->entity->VCG_raw->V3->size()<<"\n"
                           <<this->entity->VCG_raw->V4->size()<<"\n"
                             <<this->entity->VCG_raw->V5->size()<<"\n";

    VCG_T_LOOP obiekt(*this->entity->VCG_raw->V1,
                      *this->entity->VCG_raw->V2,
                      *this->entity->VCG_raw->V3,
                      *this->entity->VCG_raw->V4,
                      *this->entity->VCG_raw->V5,
                      *this->entity->VCG_raw->V6,
                      *this->entity->VCG_raw->I,
                      *this->entity->VCG_raw->II,
                      *this->entity->Waves->QRS_onset,
                      *this->entity->TWaveStart,
                      *this->entity->Waves->T_end);
    QLOG_TRACE() <<"VCG run execute";
    obiekt.Run();
QLOG_TRACE() <<"VCG ran";
    this->entity->X  = new QVector<double> (obiekt.getX());
    this->entity->Y  = new QVector<double> (obiekt.getY());
    this->entity->Z  = new QVector<double> (obiekt.getZ());
    this->entity->MA = new QVector<double> (obiekt.getMA());
    this->entity->RMMV=new QVector<double> (obiekt.getRMMV());
    this->entity->DEA= new QVector<double> (obiekt.getDEA());

for(int i=0;i<this->entity->MA->size();i++)
    QLOG_TRACE()<<this->entity->MA->at(i);
QLOG_TRACE() <<"results";
    if(this->entity->settings->signalIndex==0)
    {
        this->entity->primary = new QVector<double>(*tmp);
    }
    else
    {
        this->entity->secondary = new QVector<double>(*tmp);
    }
QLOG_TRACE() <<"previous signals loaded.";
    emit runVcgLoop_done(this->entity);
    QLOG_INFO() << "VcgLoop done";            

}

void AppController::runQtDisp()
{
    QLOG_INFO() <<"QT_DISP started.";

    ifWavesExists();
    QT_DISP obiekt;

    vector<int> qrs_on;
    vector<int> qrs_end;
    vector<int> Pwave_start;

    vector<double> baselined = (*this->entity->ecg_baselined).toStdVector();

    vector<Evaluation> output;
    vector<double> T_end;

    QVector<double>::iterator point0 = this->entity->ecg_baselined->begin();

    for(int i=0; i<this->entity->Waves->QRS_onset->size();i++)
    {
        qrs_on.push_back(this->entity->Waves->QRS_onset->at(i) - point0);
        //QLOG_TRACE() << qrs_on.at(i);
    }
    for(int i=0; i<this->entity->Waves->QRS_end->size();i++)
    {
        qrs_end.push_back(this->entity->Waves->QRS_end->at(i) - point0);
    }
    for(int i=0; i<this->entity->Waves->PWaveStart->size();i++)
    {
        Pwave_start.push_back(this->entity->Waves->PWaveStart->at(i) - point0);
    }

    obiekt.getInput(baselined, qrs_on, qrs_end, Pwave_start,
                    (double)this->entity->info->frequencyValue);
    QLOG_INFO() << "MVC/ QT_DISP reslly started.";
    obiekt.Run();    
    obiekt.setOutput(output,T_end);


    this->entity->Waves->T_end = new iters;
    for(int i=0 ; i<T_end.size();i++)
        this->entity->Waves->T_end->append(&((*this->entity->ecg_baselined)[(int) floor(T_end.at(i)*this->entity->info->frequencyValue)]));

    QLOG_TRACE() <<"MVC/ QT_DISP calculated"<<this->entity->Waves->T_end->size()<<" TWave end-points.";

    this->entity->evaluations = new QVector<Evaluation>(QVector<Evaluation>::fromStdVector(output));

    emit this->QtDisp_done(this->entity);
    QLOG_INFO() << "QT_DISP done.";

}

void AppController::runWaves()
{
    QLOG_INFO() << "Waves started.";

    ifRpeaksExists();

    deleteWaves();// to sprawdza, czy zeby nie nadpisac

    waves obiekt;
    obiekt.calculate_waves(*(this->entity->ecg_baselined),
                           *(this->entity->Rpeaks),
                           this->entity->info->frequencyValue);

    if (this->entity->Waves==NULL)
    {
        QLOG_TRACE() << "Waves was NULL before.";
        this->entity->Waves = new Waves_struct;
    }

        this->entity->Waves->QRS_onset = new iters(obiekt.get_qrs_onset());
        QLOG_INFO() << "Waves/ calculated "<<QString::number(this->entity->Waves->QRS_onset->size())
                    <<" QRS_onset points.";
        this->entity->Waves->QRS_end = new iters(obiekt.get_qrs_begin());
        QLOG_INFO() << "Waves/ calculated "<<QString::number(this->entity->Waves->QRS_end->size())
                    <<" QRS_end points.";

        this->entity->Waves->PWaveStart = new iters(obiekt.get_p_onset());
        QLOG_INFO() << "Waves/ calculated "<<QString::number(this->entity->Waves->PWaveStart->size())
                    <<" PWaveStart points.";
        //QLOG_FATAL() << "Waves/ PWaveStart not ready yet.";

        //QLOG_FATAL() << "Waves/ PWaveEnd not ready yet.";
        this->entity->Waves->PWaveEnd   = new iters(obiekt.get_p_end());
        QLOG_INFO() << "Waves/ calculated "<<QString::number(this->entity->Waves->PWaveEnd->size())
                    <<" PWaveEnd points.";

        this->entity->Waves->Count=this->entity->Waves->QRS_onset->size();
        if (this->entity->Waves->Count>this->entity->Waves->QRS_end->size())
            this->entity->Waves->Count=this->entity->Waves->QRS_end->size();
        if (this->entity->Waves->Count>this->entity->Waves->PWaveStart->size())
            this->entity->Waves->Count=this->entity->Waves->PWaveStart->size();
        if (this->entity->Waves->Count>this->entity->Waves->PWaveEnd->size())
            this->entity->Waves->Count=this->entity->Waves->PWaveEnd->size();

    emit this->Waves_done(this->entity);
    QLOG_INFO() << "Waves done.";
}

void AppController::runSigEdr()
{
    QLOG_INFO() << "SigEdr started.";

    ifWavesExists();

    if (this->entity->SigEdr_q!= NULL)
    {
        this->entity->SigEdr_q->clear();
        this->entity->SigEdr_q=NULL;
    }
    if (this->entity->SigEdr_r!= NULL)
    {
        this->entity->SigEdr_r->clear();
        this->entity->SigEdr_r=NULL;
    }

    QVector<double> *tmp_baselined = NULL;
    QVector<unsigned int> tmp_Rpeaks;
    const int edr_lead = this->entity->settings->SigEdr_lead;
    iters *Qrs_on ;
    iters *Qrs_end;


    //policzony sygnal
    (this->entity->settings->signalIndex==0) ? this->entity->settings->signalIndex=1 : this->entity->settings->signalIndex=0;
    tmp_baselined = new QVector<double>(*(this->entity->ecg_baselined));  //kopia policzonego
    runEcgBaseline();    //liczenie nowego dla drugiej elektrody

    if (this->entity->settings->SigEdr_rpeaks)
    {

        if(this->entity->ecg_baselined==NULL)
        {
            QLOG_FATAL() << "Brak danych dla SigEdr.";
            return;
        }

        //tmp_Rpeaks    = (this->entity->Rpeaks_uint); //kopia starego
        deep_copy_vect(tmp_Rpeaks,this->entity->Rpeaks_uint);

        runRPeaks();

        QLOG_TRACE()<< "SIG_EDR/ Sizes are: "<<QString::number(this->entity->ecg_baselined->size())<<" "
                    <<QString::number(tmp_baselined->size())<<" "
                   <<QString::number(this->entity->Rpeaks->size())<<" "
                   <<QString::number(tmp_Rpeaks.size())<<"."  ;

        sig_edr obiekt(*(this->entity->ecg_baselined),
                       (this->entity->Rpeaks_uint),
                       *(tmp_baselined),
                       (tmp_Rpeaks));
/*
        if (this->entity->settings->signalIndex == 1 && edr_lead ==2 )
            obiekt.new_RPeaks_signal(edr_lead,tmp_Rpeaks);
        if (this->entity->settings->signalIndex == 1 && edr_lead ==1 )
            obiekt.new_RPeaks_signal(edr_lead,this->entity->Rpeaks_uint);
        if (this->entity->settings->signalIndex == 0 && edr_lead ==1 )
            obiekt.new_RPeaks_signal(edr_lead,tmp_Rpeaks);
        if (this->entity->settings->signalIndex == 1 && edr_lead ==2 )
            obiekt.new_RPeaks_signal(edr_lead,this->entity->Rpeaks_uint);
*/
        this->entity->SigEdr_r = new QVector<double>(*(obiekt.retrieveEDR_QVec(1,this->entity->settings->SigEdr_lead)));
        QLOG_INFO() << "SigEdr_r/ calculated from RPeaks " <<QString::number(this->entity->SigEdr_r->size())<<" samples.";
    }
                    /*DLA QRS'OW************************************/

    if (this->entity->settings->SigEdr_qrs)
    {
        if((this->entity->Waves->QRS_onset==NULL) ||
            (this->entity->Waves->QRS_end == NULL))
        {
            QLOG_ERROR() << "MVC/ Sig_Edr has no QRS.";
            return;
        }
        Qrs_on = new iters(*(this->entity->Waves->QRS_onset));
        Qrs_end= new iters(*(this->entity->Waves->QRS_end));

        runWaves();

        QLOG_TRACE()<< "SIG_EDR/ Sizes are: "
                   <<QString::number(this->entity->Waves->QRS_onset->size())<<" tmp-q_on\n"
                   <<QString::number(this->entity->Waves->QRS_end->size())<<" tmp-q_end\n"
                   <<QString::number(Qrs_on->size()) << "entity-q_on\n"
                   <<QString::number(Qrs_end->size()) << "entity-q_end\n";

        sig_edr obiekt_qrs(*(this->entity->ecg_baselined),
                           *(this->entity->Waves->QRS_onset),
                           *(this->entity->Waves->QRS_end  ),
                           *tmp_baselined,
                           *Qrs_on,
                           *Qrs_end);
/*
        obiekt_qrs.new_Waves_signal(*(this->entity->Waves->QRS_onset),
                                    *(this->entity->Waves->QRS_end  ),
                                    *Qrs_on,
                                    *Qrs_end);
*/
        this->entity->SigEdr_q = new QVector<double>(*obiekt_qrs.retrieveEDR_QVec(2,edr_lead));

    }

//przywrocenie odpowiedniego sygnalu
(this->entity->settings->signalIndex==0) ? this->entity->settings->signalIndex=1 : this->entity->settings->signalIndex=0;
runEcgBaseline();
runRPeaks();
runWaves();

    QLOG_INFO() <<"SigEdr done.";
    emit this->SigEdr_done(this->entity);

}

void AppController::runHRT()
{
    QLOG_INFO() << "HRT started.";
    ifRpeaksExists();

    if (this->entity->hrt_tachogram!=NULL)
    {
        QLOG_INFO() << "MVC/ HRT already exists.";
        return;
    }
    HRT::HRTmodule obiekt;
    obiekt.calculateHRT(this->entity->Rpeaks_uint,(int)this->entity->info->frequencyValue);

    this->entity->hrt_tachogram = new QVector<double>(obiekt.get_tachogram()); //zwraca vektor reprezentujacy tachogram (25 elementow)
    this->entity->vpbs_detected_count = obiekt.get_VEBcount();//zwraca liczbę znalezionych i zaakceptowanych VEB'ow
    this->entity->turbulence_slope=	obiekt.get_TS();
    this->entity->turbulence_onset= obiekt.get_TO();
    this->entity->hrt_a	= obiekt.get_a();// zwraca wspolczynnik kierunkowy prostej
    this->entity->hrt_b	= obiekt.get_b();// ax+b - do wyrysowania na tachogramie

        emit this->HRT_done(this->entity);

    QLOG_INFO() << "HRT done.";
}

void AppController::runSleepApnea()
{
    QLOG_INFO()<< "Sleep apnea started.";

    ifRpeaksExists();    
    if ( (this->entity->SleepApnea!=NULL)
         & (this->entity->SleepApnea_plot!=NULL)
         & (this->entity->SleepApneaamp!=NULL)
         & (this->entity->SleepApneafreq!=NULL)
         & (this->entity->SleepApneatime!=NULL))
    {
        QLOG_INFO() << "MVC/ Sleep Apnea already exists.";
        return;
    }


    sleep_apnea obiekt((int)this->entity->info->frequencyValue);

    this->entity->SleepApnea = new QVector<BeginEndPair>(obiekt.sleep_apnea_output(
                                                             this->entity->Rpeaks_uint));

    //narysować proste na podstawie wartosci treshold z obiekt.gui_output
    this->entity->SleepApnea_plot = new QVector<double>(obiekt.gui_output(
                                                            this->entity->Rpeaks_uint));

    QVector<QVector<double>> *tmp = new QVector<QVector<double>>(obiekt.sleep_apnea_plots(
                                                                    this->entity->Rpeaks_uint));
    this->entity->SleepApneatime= new QVector<double>(tmp->at(0));
    this->entity->SleepApneaamp = new QVector<double>(tmp->at(1));
    this->entity->SleepApneafreq= new QVector<double>(tmp->at(2));

    for(int i =0; i<tmp->size();i++)
    {
        tmp[i].clear();
    }

    //wykorzystac obiekt.sleep_apnea_plots() do wyrysowania dwóch wykresów!!!

    for(int i =0; i< this->entity->SleepApnea->size();i++)
        QLOG_TRACE() << "Sleep Apnea/ "<<
                        (this->entity->SleepApnea->at(i).first)     <<" "
                        <<(this->entity->SleepApnea->at(i).second);
    QLOG_INFO() << "Sleep_Apnea done.";
    emit this->SleepApnea_done(this->entity);

}

//ECG BASELINE

void AppController::ecgBase_Kalman1Changed(const QString arg1)
{
    this->entity->settings->kalman_arg1 = arg1;
}
void AppController::ecgBase_Kalman2Changed(const QString arg2)
{
    this->entity->settings->kalman_arg2 = arg2;
}
void AppController::ecgBase_WindowSizeEdit(const QString arg1)
{
    this->entity->settings->avgWindowSize = arg1.toInt();
}
void AppController::ecgButterChanged(const int set_number)
{
    this->entity->settings->coeff_set = set_number;
}

//ST INTERVAL

void AppController::stInterval_detectionWidthChanged(int arg1)
{
    this->entity->settings->detect_window = arg1;
//    QLOG_INFO() << "detection width" << arg1;
}

void AppController::stInterval_smoothingWidthChanged(int arg1)
{
    this->entity->settings->smooth_window = arg1;
//    QLOG_INFO() << "smoothing width" << arg1;
}

void AppController::stInterval_morphologyChanged(double arg1)
{
    this->entity->settings->morph_coeff = arg1;
//    QLOG_INFO() << "morph" << arg1;
}

void AppController::stInterval_levelThresholdChanged(double arg1)
{
    this->entity->settings->level_tresh = arg1;
//    QLOG_INFO() << "level" << arg1;
}

void AppController::stInterval_slopeThresholdChanged(double arg1)
{
    this->entity->settings->slope_tresh = arg1;
//    QLOG_INFO() << "slope" << arg1;
}

void AppController::stInterval_algorithmChanged(int index)
{
    this->entity->settings->quadratic = (index == 1);
//    QLOG_INFO() << "algorithm" << index;
}

/************************************************************/
//useful functions
void AppController::ifEcgBaselineExists(void)
{
    if (this->entity->ecg_baselined==NULL)
        runEcgBaseline();
}
void AppController::ifRpeaksExists(void)
{
    //ifEcgBaselineExists();
    if (this->entity->Rpeaks==NULL)
        runRPeaks();
}
void AppController::ifWavesExists()
{
    //ifRpeaksExists();
    if (this->entity->Waves->Count==0)
        runWaves();
}
void AppController::ifTWaveExists()
{
    if (this->entity->Waves->T_end==NULL)
        runQtDisp();
}
void AppController::ifTWaveStartExists()
{
    if (this->entity->TWaveStart==NULL)
        runStInterval();
}

void AppController::deleteWaves(void)
{
    if (this->entity->Waves!=NULL)
    {
        if (this->entity->Waves->PWaveEnd!=NULL && !(this->entity->Waves->PWaveEnd->isEmpty()))
        {
            this->entity->Waves->PWaveEnd->clear();
            this->entity->Waves->PWaveEnd=NULL;
            QLOG_INFO() << "MVC/ delete PwaveEnd";
        }
        if (this->entity->Waves->PWaveStart!=NULL && !(this->entity->Waves->PWaveStart->isEmpty()))
        {
            this->entity->Waves->PWaveStart->clear();
            this->entity->Waves->PWaveStart=NULL;
            QLOG_INFO() << "MVC/ delete PwaveStart";
        }
        if (this->entity->Waves->QRS_onset!=NULL && !(this->entity->Waves->QRS_onset->isEmpty()))
        {
            this->entity->Waves->QRS_onset->clear();
            this->entity->Waves->QRS_onset=NULL;
            QLOG_INFO() << "MVC/ delete QRS_onset";
        }
        if (this->entity->Waves->QRS_end!=NULL && !(this->entity->Waves->QRS_end->isEmpty()))
        {
            this->entity->Waves->QRS_end->clear();
            this->entity->Waves->QRS_end=NULL;
            QLOG_INFO() << "MVC/ delete QrsEnd";
        }

        this->entity->Waves->Count=0;
     //   delete this->entity->Waves;
      //  this->entity->Waves = new Waves_struct;
    }
}

void AppController::deleteApnea()
{
    if (this->entity->SleepApnea!=NULL)
    {
        this->entity->SleepApnea->clear();
        this->entity->SleepApnea=NULL;
    }
    if (this->entity->SleepApnea_plot!=NULL)
    {
        this->entity->SleepApnea_plot->clear();
        this->entity->SleepApnea_plot=NULL;
    }
    if (this->entity->SleepApneaamp!=NULL)
    {
        this->entity->SleepApneaamp->clear();
        this->entity->SleepApneaamp=NULL;
    }
    if (this->entity->SleepApneafreq!=NULL)
    {
        this->entity->SleepApneafreq->clear();
        this->entity->SleepApneafreq=NULL;
    }
        QLOG_INFO() << "MVC/ Sleep Apnea deleted";

}

void AppController::deleteHRV1(void)
{
    if (this->entity->fft_x != NULL)
    {
        this->entity->fft_x->clear();
        this->entity->fft_x=NULL;
        QLOG_INFO() <<"MVC/ HRV1-x removed.";
    }
    if (this->entity->fft_y != NULL)
    {
        this->entity->fft_y->clear();
        this->entity->fft_y=NULL;
        QLOG_INFO() <<"MVC/ HRV1-y removed.";
    }
    if (this->entity->RR_x != NULL)
    {
        this->entity->RR_x->clear();
        this->entity->RR_x=NULL;
        QLOG_INFO() <<"MVC/ HRV1-RR_x removed.";
    }
    if (this->entity->RR_x != NULL)
    {
        this->entity->RR_x->clear();
        this->entity->RR_x=NULL;
        QLOG_INFO() <<"MVC/ HRV1-RR_y removed.";
    }
}

void AppController::load12lead_db(VCG_input &input)
{

    QLOG_TRACE() << "loading started.";
    QStringList line;
    QStringList::iterator iter_column;

    int i=0;
    QString name = "./samples.txt";

    QFile f(name);
    if (f.open(QIODevice::ReadOnly))
    {
        QLOG_TRACE() <<"VCG_LOOP/ Otwarto plik.";
        QTextStream in(&f);
        in >> name;

        while (!in.atEnd())
        {
            line=(name).split(",",QString::SkipEmptyParts);
            iter_column=line.begin();
            //iter_column++;

            input.I->append((*iter_column).toDouble());
            QLOG_TRACE() <<"I sample: " << (*iter_column).toDouble();
            iter_column++;
            input.II->append((*iter_column).toDouble());
            QLOG_TRACE() <<"II sample: " << (*iter_column).toDouble();
            iter_column++;iter_column++;iter_column++;iter_column++;iter_column++;//iter_column++;
            input.V1->append((*iter_column).toDouble());
            QLOG_TRACE() <<"v1 sample: " << (*iter_column).toDouble();
            iter_column++;
            input.V2->append((*iter_column).toDouble());
            QLOG_TRACE() <<"v2 sample: " << (*iter_column).toDouble();
            iter_column++;
            input.V3->append((*iter_column).toDouble());
            QLOG_TRACE() <<"v3 sample: " << (*iter_column).toDouble();
            iter_column++;
            input.V4->append((*iter_column).toDouble());
            iter_column++;
            input.V5->append((*iter_column).toDouble());
            iter_column++;
            input.V6->append((*iter_column).toDouble());
            QLOG_TRACE() <<"v6 sample: " << (*iter_column).toDouble();

            line.clear();
            in>>name;
            i++;
        }

        f.close();
    }
    else
        QLOG_FATAL() <<"VCG_LOOP/ Nie otwarto pliku.";

    //QLOG_TRACE() << "loading started.";

  /*//////////////////////////////////////////////////////////////////////////*/
 /*   std::ifstream input;
    try{
    input.open("D:\\VCG_T_LOOP\\dane.txt",std::ifstream::in);
    }
    catch(const std::string& msg)
    {
        QLOG_ERROR()<<QString::fromStdString(msg);
    }

    if (!input)
        QLOG_TRACE()<<"chuuj!";
      std::string line;
      QVector<double> d1;
      QVector<double> d2;
      QVector<double> d3;
      QVector<double> d4;
      QVector<double> d5;
      QVector<double> d6;
      QVector<double> i;
      QVector<double> ii;
      std::vector<std::string> samples;
      char b;
      while(getline(input,line))
      {
          samples = split(line,",");
          i.append(atof(samples[0].c_str()));
          ii.append(atof(samples[1].c_str()));
          d1.append(atof(samples[6].c_str()));
          d2.append(atof(samples[7].c_str()));
          d3.append(atof(samples[8].c_str()));
          d4.append(atof(samples[9].c_str()));
          d5.append(atof(samples[10].c_str()));
          d6.append(atof(samples[11].c_str()));
          QLOG_TRACE()<<"VCG_input loop.";
      }
      input.close();
      this->entity->VCG_raw->I=new QVector<double>(i);
      this->entity->VCG_raw->II=new QVector<double>(ii);
      this->entity->VCG_raw->V1=new QVector<double>(d1);
      this->entity->VCG_raw->V2=new QVector<double>(d2);
      this->entity->VCG_raw->V3=new QVector<double>(d3);
      this->entity->VCG_raw->V4=new QVector<double>(d4);
      this->entity->VCG_raw->V5=new QVector<double>(d5);
      this->entity->VCG_raw->V6=new QVector<double>(d6);
*/
  /*//////////////////////////////////////////////////////////////////////////*/
  QLOG_TRACE() << "loading done.";
}
vector<string> AppController::split(const string& s, const string& delim, const bool keep_empty) {
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}
