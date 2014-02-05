#include "QRS_CLASS/qrsclass.h"
#include <QDebug>
#include "QRS_CLASS/instance.h"
#include "QRS_CLASS/Extractors/abstractextractor.h"
#include "QRS_CLASS/Extractors/malinowskaextractor.h"
#include "QRS_CLASS/Extractors/speedamplitudeextractor.h"
#include "QRS_CLASS/Extractors/maxspeedexceedextractor.h"
#include "QRS_CLASS/Clusterers/abstractclusterer.h"
#include "QRS_CLASS/Clusterers/kmeans.h"
#include "QRS_CLASS/Clusterers/gmeans.h"

//include do loggera - korzystajcie smialo
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

void QRSClassModule::setDefaultConfiguration()
{
    // TMP
    KMeans* clusterer = new KMeans();
    clusterer->setMaxIterations(1000);
    clusterer->setNumberOfClusters(3);
    this->clusterer = clusterer;
}


QRSClassModule::QRSClassModule()
{
    this->ecgBaselined = NULL;
    this->waves_onset = NULL;
    this->waves_end = NULL;
    this->extractors = new QList<AbstractExtractor*>;
    this->artifactsList = NULL;
    this->runParallel = false;
}

void QRSClassModule::setClusterer(ClustererType clustererType)
{
    switch (clustererType)
    {
    case EMClusterer:
    {
        ; //miejsce na clusterera Expectation-Maximization
        break;
    }
    case KMeansClusterer:
    {
        KMeans* clusterer = new KMeans();
        clusterer->setMaxIterations(1000);
        clusterer->setNumberOfClusters(3);
        this->clusterer = clusterer;
        break;
    }
    default:
    {
        GMeans* clusterer = new GMeans();
        clusterer->setClusterNumbers(1,30);
        clusterer->setMaxIterations(10);
        this->clusterer = clusterer;
    }
    }
}

void QRSClassModule::setEGCBaseline(QVector<double> *ecg)
{
    this->ecgBaselined = ecg;
}

void QRSClassModule::setWaves(QVector<QVector<double>::const_iterator> *waves_onset, QVector<QVector<double>::const_iterator> *waves_end)
{
    this->waves_onset = waves_onset;
    this->waves_end = waves_end;
}

bool QRSClassModule::setSettings(QRSClassSettings settings)
{
    switch(settings.clusterer)
    {
        case KMeansClusterer:
        {
            KMeans* clusterer = new KMeans();
            clusterer->setMaxIterations(settings.maxIterations);
            //clusterer->setNumberOfClusters(settings.minClusterNo);
            clusterer->setNumberOfClusters(3);
            this->clusterer = clusterer;
            this->runParallel = settings.parallelExecution;
            break;
        }
        default:
        {
            GMeans* clusterer = new GMeans();
            clusterer->setClusterNumbers(3,3);
            clusterer->setMaxIterations(settings.maxIterations);
            this->runParallel = false;
            this->clusterer = clusterer;
        }
    }

    return true;
}

bool QRSClassModule::process()
{
    this->errMsg = "Everything Fine";
    if (this->ecgBaselined == NULL)
    {
        this->errMsg = "ECG SIGNAL NOT SET!";
        return false;
    }

    if (this->waves_onset == NULL || this->waves_end  == NULL)
    {
        this->errMsg = "WAVES NOT SET!";
        return false;
    }

    if (this->clusterer == NULL)
    {
        this->errMsg = "CLUSTERER NOT SET!";
        return false;
    }

    if (this->artifactsList != NULL)
    {
        delete this->artifactsList;
    }
    this->artifactsList = new QList<int>();

    // Feature Extraction
    extractors->append(new MalinowskaExtractor());
    extractors->append(new SpeedAmplitudeExtractor());
    extractors->append(new MaxSpeedExceedExtractor());

    // Checking if each onset has a corresponding end and vice versa
    int completeQRScount = 0;
    int offsetAdditionalEnd = 0;

    if (this->waves_onset->count() >= this->waves_end->count()) //more onsets - ignore the last one by having only the ends counted, or don't care if they're equal
    {
        completeQRScount = this->waves_end->count();
    }
    else //less onsets - ignore the leading end by offseting the index by 1
    {
        completeQRScount = this->waves_onset->count();
        offsetAdditionalEnd = 1;
    }

    QList<Instance>* features = new QList<Instance>();
    for(int i = 0; i < completeQRScount; i++)
    {
        QList<double> currentQRS;

        int currentQRSfirstSampleNumber = this->waves_onset->at(i) - this->ecgBaselined->begin();
        int currentQRSlastSampleNumber = this->waves_end->at(i+offsetAdditionalEnd) - this->ecgBaselined->begin();

        for(int j = currentQRSfirstSampleNumber; j <= currentQRSlastSampleNumber; j++)
        {
            currentQRS.append(this->ecgBaselined->at(j));
        }

        if (currentQRS.count() < 2)
        {
            this->artifactsList->append(currentQRSfirstSampleNumber); //chyba ok
        }

        Instance currInstance(extractors->count());

        int j = 0;

        foreach(AbstractExtractor* extractor, *extractors)
        {
            currInstance[j] = extractor->extractFeature(currentQRS);
            j++;
        }
        features->append(currInstance);
    }

    // Cluster
    this->clusterer->setClusteringSet(features);

    if (!this->clusterer->classify())
    {
        this->errMsg = this->clusterer->getErrorMessage();
        return false;
    }

    return true;
}

QVector<QRSClass> *QRSClassModule::getClasses()
{
    QVector<QRSClass> *toReturn = new QVector<QRSClass>();

    int classNo = 0;
    foreach(Instance currClass , *(this->clusterer->getClasses()))
    {
        QRSClass currentClass;
        currentClass.features = new QList<double>();
        currentClass.featureNames = new QList<QString>();
        currentClass.featureTooltip = new QList<QString>();
        currentClass.representative = new QVector<double>();
        currentClass.classMembers = new QList<int>();

        for(int i = 0 ; i < currClass.numberOfAttributes(); i++)
        {
            currentClass.features->append(currClass[i]);
            currentClass.featureNames->append(extractors->at(i)->getName());
            currentClass.featureTooltip->append(extractors->at(i)->getTooltip());
        }

        foreach(int sampleNo, *(this->clusterer->getClassMembers(classNo)))
        {
            if (!this->artifactsList->contains(sampleNo))
                currentClass.classMembers->append(sampleNo);
        }

        int representativeId = this->clusterer->getClassRepresentative(classNo);

        if (representativeId > -1)
        {
            // Checking if each onset has a corresponding end and vice versa
            int offsetAdditionalEnd = 0;

            if (this->waves_onset->count() < this->waves_end->count()) //less onsets - ignore the leading end by offseting the index by 1
            {
                offsetAdditionalEnd = 1;
            }
            int begin = waves_onset->at(representativeId) - this->ecgBaselined->begin();
            int end = waves_end->at(representativeId+offsetAdditionalEnd) - this->ecgBaselined->begin();
            for(int j = begin; j < end; j++)
                currentClass.representative->append(ecgBaselined->at(j));
        }
        else
        {
            for(int j = 0; j < 30; j++)
                currentClass.representative->append(j);
        }
        currentClass.classLabel = QString("Class ").append(QString::number(classNo));

        if (currentClass.classMembers->count() > 0)
        {
            toReturn->append(currentClass);
        }

        classNo++;
    }

    QRSClass artifactClass;
    artifactClass.classLabel = QString("Artifacts");
    artifactClass.features = new QList<double>();
    artifactClass.featureNames = new QList<QString>();
    artifactClass.featureTooltip = new QList<QString>();
    artifactClass.representative = new QVector<double>();
    artifactClass.classMembers = new QList<int>();

    for(int i = 0 ; i < extractors->count(); i++)
    {
        artifactClass.features->append(0);
        artifactClass.featureNames->append(extractors->at(i)->getName());
        artifactClass.featureTooltip->append(extractors->at(i)->getTooltip());
    }

    artifactClass.representative->append(0);

    for(int i = 0 ; i < this->artifactsList->count(); i++)
    {
        artifactClass.classMembers->append(this->artifactsList->at(i));
    }

    QList<int> *artifacts = clusterer->getArtifacts();

    for(int i = 0; i < artifacts->count(); i++)
    {
        if (!artifactClass.classMembers->contains(artifacts->at(i)))
            artifactClass.classMembers->append(artifacts->at(i));
    }

    toReturn->append(artifactClass);

    return toReturn;
}

QString QRSClassModule::getErrorMessage()
{
    return this->errMsg;
}
