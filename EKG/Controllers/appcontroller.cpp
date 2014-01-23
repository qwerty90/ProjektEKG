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
    this->connect(view, SIGNAL(switchTWA(unsigned char)), this, SLOT(switchTWA(unsigned char)));

    this->connect(view, SIGNAL(qrsClassChanged(int,int)),this,SLOT(sendQRSData(int,int)));
    this->connect(this, SIGNAL(sendQRSData(QRSClass,int)),view,SLOT(receiveQRSData(QRSClass,int)));

    this->connect(view, SIGNAL(ecgBase_CzasUsrednieniaChanged(QString)),this,SLOT(CzasUsrednieniaEdit(QString)));
    this->connect(view, SIGNAL(ecgBase_Kalman1Changed(QString)),this,SLOT(ecgBase_Kalman1Changed(QString)));
    this->connect(view, SIGNAL(ecgBase_Kalman2Changed(QString)),this,SLOT(ecgBase_Kalman2Changed(QString)));
    this->connect(view, SIGNAL(ecgBase_CzestotliwoscProbkowaniaChanged(QString)),this,SLOT(ecgBase_WindowSizeEdit(QString)));

    this->connect(view, SIGNAL(on_st_interval_detection_width_Changed(const QString &)),this,SLOT(on_st_interval_detection_width_Changed(const QString &)));
    this->connect(view, SIGNAL(on_st_interval_smothing_width_Changed(const QString &)),this,SLOT(on_st_interval_smothing_width_Changed(const QString &)));
    this->connect(view, SIGNAL(on_st_interval_morphology_Changed(const QString &)),this,SLOT(on_st_interval_morphology_Changed(const QString &)));
    this->connect(view, SIGNAL(on_st_interval_level_threshold_Changed(const QString &)),this,SLOT(on_st_interval_level_threshold_Changed(const QString &)));
    this->connect(view, SIGNAL(on_st_interval_slope_threshold_Changed(const QString &)),this,SLOT(on_st_interval_slope_threshold_Changed(const QString &)));
    this->connect(view, SIGNAL(switchDetectionAlgorithmType_ST_INTERVAL(int)),this,SLOT(switchDetectionAlgorithmType_ST_INTERVAL(int)));

    this->connect(view, SIGNAL(runEcgBaseline()),this, SLOT (runEcgBaseline()));//example
    this->connect(view, SIGNAL(runAtrialFibr()) ,this, SLOT (runAtrialFibr()));
    this->connect(view, SIGNAL(runStInterval()) ,this, SLOT (runStInterval()));
    this->connect(view, SIGNAL(runHRV1())       ,this, SLOT (runHRV1()));
    this->connect(view, SIGNAL(runRPeaks())     ,this, SLOT (runRPeaks()));
    this->connect(view, SIGNAL(runWaves())      ,this, SLOT (runWaves()));
    this->connect(view, SIGNAL(runSigEdr())     ,this, SLOT (runSigEdr()));
    this->connect(view, SIGNAL(runVcgLoop())     ,this, SLOT (runVcgLoop()));

    this->connect(view, SIGNAL(run()), this, SLOT(run()));

    this->connect(this, SIGNAL(EcgBaseline_done(EcgData*)),view, SLOT(drawEcgBaseline(EcgData*)));//example
    this->connect(this, SIGNAL( AtrialFibr_done(EcgData*)),view, SLOT(drawAtrialFibr(EcgData*)));
    this->connect(this, SIGNAL(StInterval_done(EcgData*)) ,view, SLOT(drawStInterval(EcgData*)));
    this->connect(this, SIGNAL(HRV1_done(EcgData*))       ,view, SLOT(drawHrv1(EcgData*)))      ;
    this->connect(this, SIGNAL(RPeaks_done(EcgData*))     ,view, SLOT(drawRPeaks(EcgData*)))    ;
    this->connect(this, SIGNAL(Waves_done(EcgData*))      ,view, SLOT(drawWaves(EcgData*)))     ;
    this->connect(this, SIGNAL(SigEdr_done(EcgData*))     ,view, SLOT(drawSigEdr(EcgData*)))    ;
    this->connect(this, SIGNAL(QrsClass_done(EcgData*))   ,view, SLOT(drawQrsClass(EcgData*)))  ;
    this->connect(this, SIGNAL(runVcgLoop_done(EcgData*))   ,view, SLOT(drawVcgLoop(EcgData*)))  ;

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
    //if(this->entity)
        //this->ResetModules();
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

/*void AppController::sendQRSData(int index, int type)
{

}*/

void AppController::switchTWA(unsigned char type)
{
    if(this->entity)
        this->entity->TWA_mode = type;
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


    deleteWaves();

    QLOG_INFO() << "All removed.";
}

void AppController::runEcgBaseline()
{
    QLOG_INFO() <<"Ecg baseline started.";
    //QVector<double> test;
    //test << 0.5 << 0.5 << 0.5;
    KalmanFilter kalman;
    const QVector<ButterCoefficients> coeff = predefinedButterCoefficientSets();

    if (this->entity->ecg_baselined!=NULL)
    {
        this->entity->ecg_baselined->clear();
    }

    switch (this->entity->settings->EcgBaselineMode)
    {
    case 0: //butterworth
        QLOG_INFO() << "BASELINE/ Using butterworth filter.";
        this->entity->ecg_baselined = new QVector<double>(processButter(*(this->entity->GetCurrentSignal()),coeff[0]));
        break;
    case 1:
        QLOG_INFO() << "BASELINE/ Using moving average filter.";
        if(this->entity->settings->averaging_time!=0)
        {
            QLOG_INFO() << "BASELINE/ Using moving average filter with averaging time = "
                        << QString::number(this->entity->settings->averaging_time) << " .";
            this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),
                                                                            (int)(this->entity->info->frequencyValue),
                                                                            this->entity->settings->averaging_time));
            this->entity->characteristics = new QVector<QPointF>(movAvgMagPlot((int)(this->entity->info->frequencyValue),
                                                                               this->entity->settings->averaging_time));
        }
        else if (this->entity->settings->avgWindowSize!=0)
        {
            QLOG_INFO() << "BASELINE/ Using moving average filter with window width = "
                        << QString::number(this->entity->settings->avgWindowSize) << " .";
            this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),
                                                                            this->entity->settings->avgWindowSize));
        }
        else
        {
            QLOG_INFO() << "BASELINE/ Using moving average filter with default window width = 3." ;
            this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),3));
        }

        break;
    case 2: //savitzky-golay
        QLOG_INFO() << "BASELINE/ Using Savitzky-Golay filter.";
        this->entity->ecg_baselined = new QVector<double>(processSGolay(*(this->entity->GetCurrentSignal())));
        break;
    case 3:  //kalman
        QLOG_INFO() << "BASELINE/ Using kalman filter.";
        this->entity->ecg_baselined = new QVector<double>(kalman.processKalman(*(this->entity->GetCurrentSignal())));
        break;
    default:
        QLOG_INFO() << "BASELINE/ Using default filter.";
        this->entity->ecg_baselined = new QVector<double>(processMovAvg(*(this->entity->GetCurrentSignal()),3));
        break;
    }            

    QLOG_INFO() << "Ecg baseline done.";

    emit EcgBaseline_done(this->entity);
}

void AppController::runHRV1()
{
    QLOG_INFO() << "HRV1 started.";
    ifRpeaksExists();

    if (this->entity->fft_x != NULL)
    {
        this->entity->fft_x->clear();
        this->entity->fft_x=NULL;
    }
    if (this->entity->fft_y != NULL)
    {
        this->entity->fft_y->clear();
        this->entity->fft_y=NULL;
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
    this->entity->LFHF = results_freq.LFHF;

    QLOG_INFO() << "HRV1 frequency done.";

    emit this->HRV1_done(this->entity);
    results_freq.xData->clear();
    results_freq.yData->clear();
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

    AtrialFibrApi obiekt(*(this->entity->ecg_baselined),
                         *(this->entity->Rpeaks) ,
                         *(this->entity->Waves->PWaveStart) )   ;

    this->entity->PWaveOccurenceRatio= obiekt.GetPWaveOccurenceRatio();
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
    QLOG_INFO() << "Run RPeaks" ;

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
    analyzer.setAlgorithm(ST_QUADRATIC);
        else
    analyzer.setAlgorithm(ST_LINEAR);

    analyzer.setDetectionSize(this->entity->settings->detect_window);
    analyzer.setSmoothSize(this->entity->settings->smooth_window);
    analyzer.setLevelThreshold(this->entity->settings->level_tresh);
    analyzer.setSlopeThreshold(this->entity->settings->slope_tresh);
    analyzer.setMorphologyCoeff(this->entity->settings->morph_coeff);

    ifWavesExists();
    if (this->entity->Waves->T_end == NULL)
    {
        QLOG_FATAL() << "ST_INTERVAL/ no Twave_end for me!";
        return;
    }

    QList<EcgStDescriptor> result;

    result = analyzer.analyze(*(this->entity->ecg_baselined),
                              *(this->entity->Rpeaks),
                              *(this->entity->Waves->QRS_end),
                              *(this->entity->Waves->T_end),
                              (double)this->entity->info->frequencyValue);

    this->entity->STintervals = new QList<EcgStDescriptor>(result);

    emit StInterval_done(this->entity);
    QLOG_INFO() << "StInterval done";
}

void AppController::runQrsClass()
{
    QLOG_INFO() << "Start QrsClass";

    if (!this->entity || !this->entity->Waves || !this->entity->ecg_baselined)
    {
        QLOG_ERROR() <<"No data for QRS_Class";
        return;
    }

    QRSClassModule QrsClassifier;
    //QrsClassifier.setSettings(this->entity->settings->qrsClassSettings);

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
    QLOG_INFO() << "Start VcgLoop (not ready yet)";



    emit runVcgLoop_done(this->entity);
    QLOG_INFO() << "VcgLoop done";
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
        this->entity->Waves->QRS_end = new iters(obiekt.get_qrs_onset());
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
 /*DLA QRS'OW****************************************************************/

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
                  <<QString::number(Qrs_on->size()) << "entity-q_on\n"  ;

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

//ECG BASELINE

void AppController::ecgBase_Kalman1Changed(const QString arg1)
{
    this->entity->settings->kalman_arg1 = arg1;
}
void AppController::ecgBase_Kalman2Changed(const QString arg2)
{
    this->entity->settings->kalman_arg2 = arg2;
}
void AppController::CzasUsrednieniaEdit(const QString arg1)
{
    this->entity->settings->averaging_time = arg1.toDouble();
}
void AppController::ecgBase_WindowSizeEdit(const QString arg1)
{
    this->entity->settings->avgWindowSize = arg1.toInt();
}

//ST INTERVAL

void AppController::on_st_interval_detection_width_Changed(const QString &arg1)
{
    this->entity->settings->detect_window = arg1.toDouble();
}

void AppController::on_st_interval_smothing_width_Changed(const QString &arg1)
{
    this->entity->settings->smooth_window = arg1.toDouble();
}

void AppController::on_st_interval_morphology_Changed(const QString &arg1)
{
    this->entity->settings->morph_coeff = arg1.toDouble();
}

void AppController::on_st_interval_level_threshold_Changed(const QString &arg1)
{
    this->entity->settings->level_tresh = arg1.toDouble();
}

void AppController::on_st_interval_slope_threshold_Changed(const QString &arg1)
{
    this->entity->settings->slope_tresh = arg1.toDouble();
}

void AppController::switchDetectionAlgorithmType_ST_INTERVAL(int index)
{

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

void AppController::deleteWaves(void)
{
    QLOG_INFO() << "MVC/ delete Waves only";
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
