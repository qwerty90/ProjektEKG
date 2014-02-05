#include "kmeans.h"
#include <limits.h>
#include <float.h>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <QDebug>

KMeans::KMeans()
{
    this->numOfClusters = 2;
    this->centroids = NULL;
    this->maxIters = 1000;
}

KMeans::~KMeans()
{
    delete this->centroids;
}

bool KMeans::classify()
{
    if (this->instances == NULL)
    {
        this->errMsg = "Instances not set";
        return false;
    }

    //
    this->errMsg = "jebut";
    return false;
    //

    //Generate initial centroids;
    srand(time(NULL));
    int numOfAttributes = this->instances->first().numberOfAttributes();
    Instance* initCentroids = new Instance[this->numOfClusters];
    Instance* newCentroids = new Instance[this->numOfClusters];

    int* usedIndexes = new int[this->numOfClusters];

    if (this->initCentroids)
    {
        QList<Instance>* tmpCentroids = this->initializeCentroids(RandomPoints,this->numOfClusters);

        for(int i = 0; i < this->numOfClusters; i++)
        {
            initCentroids[i] = Instance(tmpCentroids->at(i));
            newCentroids[i] = Instance(numOfAttributes);
        }
    } else {
        if (this->centroids == NULL || this->centroids->count() != this->numOfClusters || this->centroids->first().numberOfAttributes() != numOfAttributes)
        {
            this->errMsg = "Centroids don't match Clustering set";
            return false;
        }

        for(int i = 0; i < this->numOfClusters; i++)
        {
            initCentroids[i] = this->centroids->at(i);
            newCentroids[i] = Instance(numOfAttributes);
        }
    }

    int* centroidsCount = new int[this->numOfClusters];

    for(int i = 0; i < this->numOfClusters; i++)
    {
        centroidsCount[i] = 0;
    }

    // K-Means
    bool keepGoing = true;
    int iter = 0;
    while(keepGoing && iter < this->maxIters)
    {
        for(int i = 0; i < this->numOfClusters; i++)
        {
            centroidsCount[i] = 0;
        }
        iter++;
        for(int i = 0; i < this->instances->count(); i++)
        {
            Instance currInstance = this->instances->at(i);

            double min = DBL_MAX;//std::numeric_limits<double>::max;
            int clusterNo = 0;

            for(int j = 0 ; j < this->numOfClusters; j++)
            {
                Instance currCentroid = initCentroids[j];
                // calc distance
                double dist = 0;

                for(int k = 0; k < currInstance.numberOfAttributes(); k++)
                {
                    dist += pow( currCentroid[k] - currInstance[k], 2 );
                }

                if (dist < min)
                {
                    min = dist;
                    clusterNo = j;
                }
            }

            // Add to winner

            centroidsCount[clusterNo]++;
            for(int j = 0; j < currInstance.numberOfAttributes(); j++)
            {
                newCentroids[clusterNo][j] += currInstance[j];
            }
        }

        // Centroids moved?
        keepGoing = false;

        for(int i = 0 ; i < this->numOfClusters; i++)
        {
            if (centroidsCount[i] == 0)
            {
                int index = rand() % this->instances->size();
                Instance inst = this->instances->at(index);
                newCentroids[i] = inst;
                keepGoing = true;
                continue;
            }

            for(int j = 0 ; j < numOfAttributes; j++)
            {
               newCentroids[i][j] /= centroidsCount[i];
                if (!keepGoing && newCentroids[i][j] != initCentroids[i][j])
                {
                    keepGoing = true;
                }

                initCentroids[i][j] = newCentroids[i][j];
            }
        }
    }

    QList<Instance>* finalCentroids = new QList<Instance>();

    for (int i = 0; i < this->numOfClusters; i++)
    {
        finalCentroids->append(initCentroids[i]);
    }

    this->itersPerformed = iter;
    this->centroids = finalCentroids;


    this->handleArtifacts();

    return true;
}

void KMeans::setNumberOfClusters(int num)
{
    this->numOfClusters = num;
    this->centroids = NULL;
    this->initCentroids = true;
}

QList<Instance> *KMeans::getClasses()
{
    return this->centroids;
}

QList<int> *KMeans::getClassMembers(int classNumber)
{
    if (classNumber < this->centroids->count())
    {
        QList<int> *toReturn = new QList<int>();
        Instance centroid = this->centroids->at(classNumber);

        int instanceNo = 0;

        foreach(Instance currInstance, *this->instances)
        {
            double distance = 0;

            for(int i = 0; i < currInstance.numberOfAttributes(); i++)
            {
                distance += pow(currInstance[i] - centroid[i],2);
            }

            for(int i = 0; i < this->numOfClusters; i++)
            {
                double clusterDist = 0;
                Instance currCluster = this->centroids->at(i);

                for(int j = 0; j < currInstance.numberOfAttributes(); j++)
                {
                    clusterDist += pow(currInstance[j] - currCluster[j],2);
                }

                if (clusterDist < distance)
                {
                    distance = -1;
                    break;
                }
            }

            if (distance > 0)
                toReturn->append(instanceNo);

            instanceNo++;
        }
        return toReturn;
    }
}

int KMeans::getClassRepresentative(int classNumber)
{
    if (classNumber < this->centroids->count())
    {
        int toReturn = -1;
        Instance centroid = this->centroids->at(classNumber);

        int instanceNo = 0;
        double minDistance = DBL_MAX;

        foreach(Instance currInstance, *this->instances)
        {
            double distance = 0;

            for(int i = 0; i < currInstance.numberOfAttributes(); i++)
            {
                distance += pow(currInstance[i] - centroid[i],2);
            }

            for(int i = 0; i < this->numOfClusters; i++)
            {
                double clusterDist = 0;
                Instance currCluster = this->centroids->at(i);

                for(int j = 0; j < currInstance.numberOfAttributes(); j++)
                {
                    clusterDist += pow(currInstance[j] - currCluster[j],2);
                }

                if (clusterDist < distance)
                {
                    distance = -1;
                    break;
                }
            }

            if (distance > 0 && distance < minDistance)
            {
                toReturn = instanceNo;
                minDistance = distance;
            }

            instanceNo++;
        }
        return toReturn;
    }
}

QList<Instance> *KMeans::getCentroids()
{
    return this->centroids; //kopia pomoze?
}

QList<Instance> KMeans::getInstancesForCentroid(int centroidIndex)
{
    if (centroidIndex < this->centroids->count())
    {
        QList<Instance> toReturn;
        Instance centroid = this->centroids->at(centroidIndex);

        foreach(Instance currInstance, *this->instances)
        {
            double distance = 0;

            for(int i = 0; i < currInstance.numberOfAttributes(); i++)
            {
                distance += pow(currInstance[i] - centroid[i],2);
            }

            for(int i = 0; i < this->numOfClusters; i++)
            {
                double clusterDist = 0;
                Instance currCluster = this->centroids->at(i);

                for(int j = 0; j < currInstance.numberOfAttributes(); j++)
                {
                    clusterDist += pow(currInstance[j] - currCluster[j],2);
                }

                if (clusterDist < distance)
                {
                    distance = -1;
                    break;
                }
            }

            if (distance > 0)
                toReturn.append(currInstance);
        }

        return toReturn;
    }
}

int KMeans::getNumberOfClusters()
{
    return this->numOfClusters;
}
