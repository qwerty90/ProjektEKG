#ifndef ABSTRACTCLUSTERER_H
#define ABSTRACTCLUSTERER_H

#include "QRS_CLASS/instance.h"
#include <QList>
#include <QString>

class AbstractClusterer
{
    QList<Instance>* initializeKMeansPPCentroids(int numberOfClusters);
    QList<Instance>* initializeRandomPointsCentroids(int numberOfClusters);
protected:
    enum InitTypes
    {
        RandomPoints,
        KMeansPP
    };

    QList<Instance>* instances;
    QList<Instance>* centroids;
    QList<int>* artifacts;
    bool initCentroids;
    int maxIters;
    QString errMsg;
    int itersPerformed;    
    int numOfClusters;

    QList<Instance>* initializeCentroids(InitTypes type, int numberOfClusters);
public:
    AbstractClusterer();
    virtual bool classify() = 0;
    //virtual bool classifyParallel() = 0;
    void handleArtifacts();
    QList<int> *getArtifacts();
    virtual QList<Instance>* getClasses() = 0;
    virtual QList<int>* getClassMembers(int classNumber) = 0;
    virtual int getClassRepresentative(int classNumber) = 0;
    virtual void setInitCentroids(bool flag);
    virtual void setCentroids(QList<Instance>* centroids);
    void setClusteringSet(QList<Instance> *set);
    void setMaxIterations(int iters);
    QString getErrorMessage();
    int getPerformedIterations();
};


#endif // ABSTRACTCLUSTERER_H
