#include "abstractclusterer.h"
#include <time.h>
#include <float.h>
#include <cmath>

#include <QDebug>

//include do loggera - korzystajcie smialo
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

AbstractClusterer::AbstractClusterer()
{
    this->instances = NULL;
    this->artifacts = NULL;
}

void AbstractClusterer::handleArtifacts()
{
    // Count std for every class
    QList<Instance> classesStd;
    QList<int> classMembersCount;
    QList<int> instanceToClass;

    int numOfAtr = this->centroids->first().numberOfAttributes();

    for(int i = 0; i < this->centroids->count(); i++)
    {
        classesStd.append(Instance(numOfAtr));
        classMembersCount.append(0);
    }

    foreach(Instance currInstance, *this->instances)
    {
        double minDistance = DBL_MAX;
        int clusterNo = 0;

        for(int i = 0; i < this->numOfClusters; i++)
        {
            double clusterDist = 0;
            Instance currCluster = this->centroids->at(i);

            for(int j = 0; j < currInstance.numberOfAttributes(); j++)
            {
                clusterDist += pow(currInstance[j] - currCluster[j],2);
            }

            if (clusterDist < minDistance)
            {
                minDistance = clusterDist;
                clusterNo = i;
            }
        }

        instanceToClass.append(clusterNo);
        classMembersCount.replace(clusterNo,classMembersCount.at(clusterNo) + 1);
        Instance mean = classesStd.at(clusterNo);
        Instance centroid = this->centroids->at(clusterNo);

        for(int i = 0; i < centroid.numberOfAttributes(); i++)
        {
            mean[i] += pow(currInstance[i] - centroid[i],2);
        }

        classesStd.replace(clusterNo, mean);
    }

    for(int i = 0; i < this->centroids->count(); i++)
    {
        Instance stDev = classesStd.at(i);

        for(int j = 0; j < stDev.numberOfAttributes(); j++)
        {
            stDev[j] = sqrt(stDev[j]/(classMembersCount.at(i) - 1));
        }

        classesStd.replace(i,stDev);
    }

    //Find artifacts
    if (this->artifacts != NULL)
        delete this->artifacts;

    this->artifacts = new QList<int>();

    for(int i = 0 ; i < instanceToClass.count(); i++)
    {
        Instance stdDev = classesStd.at(instanceToClass.at(i));
        Instance currInstance = instances->at(i);
        Instance centroid = centroids->at(instanceToClass.at(i));

        for(int j = 0; j < stdDev.numberOfAttributes(); j++)
        {
            if (2*stdDev[j] < centroid[j] - currInstance[j])
            {
                this->artifacts->append(i);
                break;
            }
        }
    }
}

QList<int> *AbstractClusterer::getArtifacts()
{
    return this->artifacts;
}

void AbstractClusterer::setMaxIterations(int iters)
{
    maxIters = iters;
}

QString AbstractClusterer::getErrorMessage()
{
    return this->errMsg;
}

int AbstractClusterer::getPerformedIterations()
{
    return this->itersPerformed;
}

void AbstractClusterer::setClusteringSet(QList<Instance> *set)
{
    this->instances = set;
}

void AbstractClusterer::setCentroids(QList<Instance> *centroids)
{
    this->centroids = centroids;
    this->initCentroids = false;
    this->numOfClusters = centroids->count();
}

void AbstractClusterer::setInitCentroids(bool flag)
{
    this->initCentroids = flag;
}

QList<Instance> *AbstractClusterer::initializeKMeansPPCentroids(int numberOfClusters)
{
    srand(time(NULL));

    // First centroid from Uniform distribution
    QList<Instance>* initCentroids = new QList<Instance>();
    int index = rand() % this->instances->size();
    initCentroids->append(this->instances->at(index));

    qDebug() << "KMeans++ start" << initCentroids->count();
    // KMeans++ Algorithm
    while(initCentroids->size() < numberOfClusters)
    {
        double propabilitySum = 0.0;
        QList<double> propDistribution;
        // Calc Propability Distribution

        foreach(Instance currInstance, *(this->instances))
        {
            double minDistance = DBL_MAX;

            foreach(Instance currCentroid, *initCentroids)
            {
                double currDistance = 0.0;

                for(int i = 0; i < currCentroid.numberOfAttributes(); i++)
                {
                    currDistance += pow(currInstance[i] - currCentroid[i],2);
                }

                if (currDistance < minDistance)
                {
                    minDistance = currDistance;
                }

            }

            minDistance = pow(minDistance,2);

            propDistribution.append(minDistance);
            propabilitySum += minDistance;
        }

        // Generate Rand in (O, propabilitySum)
        double prob = ((double)rand() / RAND_MAX)*propabilitySum;

        qDebug() << prob << propabilitySum;

       // Choose Sample to become centroid
        double currentProb = 0.0;
        for(int i = 0; i < propDistribution.count(); i++)
        {
            currentProb += propDistribution.at(i);

            if (prob < currentProb)
            {
                initCentroids->append(instances->at(i));
                break;
            }
        }
    }

    return initCentroids;
}

QList<Instance> *AbstractClusterer::initializeRandomPointsCentroids(int numberOfClusters)
{
    srand(time(NULL));
    QList<Instance>* initCentroids = new QList<Instance>();

    int* usedIndexes = new int[numberOfClusters];

    for(int i = 0; i < numberOfClusters; i++)
    {
        bool alreadyInUse = true;
        int index;

        while (alreadyInUse)
        {
            alreadyInUse = false;
            index = rand() % this->instances->size() - 1;

            for(int j = 0 ; j < numberOfClusters; j++)
            {
                if (index == usedIndexes[j])
                {
                    alreadyInUse = true;
                    break;
                }
            }
        }

        usedIndexes[i] = index;
        initCentroids->append(this->instances->at(index));
    }

    return initCentroids;
}

QList<Instance> *AbstractClusterer::initializeCentroids(AbstractClusterer::InitTypes type, int numberOfClusters)
{
    return this->initializeRandomPointsCentroids(numberOfClusters);
}
