#ifndef QRSCLASS_H
#define QRSCLASS_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QList>

#include "waves/src/waves.h"

class AbstractExtractor;
class AbstractClusterer;

enum ClustererType
{
    KMeansClusterer,
    GMeansClusterer,
    EMClusterer
};

struct QRSClassSettings {
    ClustererType clusterer;
    int maxIterations;
    int minClusterNo;
    int maxClusterNo;
    bool parallelExecution;
    int insideIterations;
};

struct QRSClass {
    QString classLabel;
    QList<double> *features;
    QList<QString> *featureNames;
    QList<QString> *featureTooltip;
    QVector<double> *representative;
    QList<int> *classMembers;
};

class QRSClassModule {
    QList<AbstractExtractor*> *extractors;
    AbstractClusterer *clusterer;
    //ClustererType clusterer;
    QString errMsg;
    QVector<double> *ecgBaselined;
    QVector<const double> *waves_onset;
    QVector<const double> *waves_end;
    QList<int> *artifactsList;
    bool runParallel;
    void setDefaultConfiguration();
public:
    QRSClassModule();
    void setClusterer(ClustererType clustererType);
    void setEGCBaseline(QVector<double> *ecg);
    void setWaves(QVector<const double*>*&, QVector<const double*>*&);
    bool setSettings(QRSClassSettings settings);
    bool process();
    QVector<QRSClass>* getClasses();
    QString getErrorMessage();
};

#endif // QRSCLASS_H
