#ifndef QRSCLASS_H
#define QRSCLASS_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QList>

#if defined(QRSCLASS_LIBRARY)
#  define QRSCLASSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QRSCLASSSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "waves.h"

class AbstractExtractor;
class AbstractClusterer;


enum ClustererType
{
    KMeansClusterer,
    GMeansClusterer
};

struct QRSCLASSSHARED_EXPORT QRSClassSettings {
    ClustererType clusterer;
    int maxIterations;
    int minClusterNo;
    int maxClusterNo;
    bool parallelExecution;
    int insideIterations;
};

struct QRSCLASSSHARED_EXPORT QRSClass {
    QString classLabel;
    QList<double> *features;
    QList<QString> *featureNames;
    QList<QString> *featureTooltip;
    QVector<double> *representative;
    QList<int> *classMembers;
};

class QRSCLASSSHARED_EXPORT QRSClassModule {
    QList<AbstractExtractor*> *extractors;
    AbstractClusterer *clusterer;
    //ClustererType clusterer;
    QString errMsg;
    QList<double> *ecgBaselined;
    QList<Waves::EcgFrame*> *waves;
    QList<int> *artifactsList;
    bool runParallel;
    void setDefaultConfiguration();
public:
    QRSClassModule();
    void setClusterer(ClustererType clustererType);
    void setEGCBaseline(QList<double> *ecg);
    void setWaves(QList<Waves::EcgFrame*> *waves);
    bool setSettings(QRSClassSettings settings);
    bool process();
    QList<QRSClass>* getClasses();
    QString getErrorMessage();
};

#endif // QRSCLASS_H
