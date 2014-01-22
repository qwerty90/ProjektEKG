#ifndef GMEANS_H
#define GMEANS_H

#include "QRS_CLASS/Clusterers/abstractclusterer.h"
#include <QList>

class GMeans : public AbstractClusterer
{
    int minNumOfClusters;
    int maxNumOfClusters;
public:
    GMeans();
    bool classify();
    bool classifyParallel();
    bool setClusterNumbers(int min, int max);
    QList<Instance>* getClasses();
    QList<int>* getClassMembers(int classNumber);
    int getClassRepresentative(int classNumber);
    QList<Instance>* getCentroids();
    int getNumberOfClusters();

    static double getNormalCDF(double x);
};

#endif // GMEANS_H
