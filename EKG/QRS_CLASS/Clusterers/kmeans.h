#ifndef KMEANS_H
#define KMEANS_H

#include "QRS_CLASS/Clusterers/abstractclusterer.h"
#include <QList>

class KMeansParallelHelper
{
    QList<Instance> *block_instances;
    Instance* block_centroids;
    int numberOfClusters;
public:
    Instance* centroids;
    int* centroidsCount;
};

class KMeans : public AbstractClusterer
{
public:
    KMeans();
    ~KMeans();
    bool classify();
    void setNumberOfClusters(int num);
    QList<Instance>* getClasses();
    QList<int>* getClassMembers(int classNumber);
    int getClassRepresentative(int classNumber);
    QList<Instance>* getCentroids();
    QList<Instance> getInstancesForCentroid(int i);
    int getNumberOfClusters();
};

#endif // KMEANS_H
