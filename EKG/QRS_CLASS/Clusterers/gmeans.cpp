#include "gmeans.h"
#include "kmeans.h"
#include "float.h"
#include <math.h>
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

#define CDF_A_1  0.254829592
#define CDF_A_2  -0.284496736
#define CDF_A_3   1.421413741
#define CDF_A_4  -1.453152027
#define CDF_A_5   1.061405429
#define CDF_P   0.3275911



GMeans::GMeans()
    : minNumOfClusters(1),
      maxNumOfClusters(10)
{
}

bool GMeans::classify()
{

    if (this->instances == NULL || this->maxNumOfClusters == 0)
        return false;
QLOG_INFO() << "dupa1";
    //Generate initial centroids;
    int numOfAttributes = this->instances->first().numberOfAttributes();
    QLOG_INFO() << "dupa1.1";
    QLOG_INFO() << "dupa1.2" << this->minNumOfClusters;
    QList<Instance>* initCentroids = this->initializeCentroids(RandomPoints, this->minNumOfClusters); //tu sie sypie

    bool keepGoing = true;
    int iters = 0;

    int currentNoOfClusters = this->minNumOfClusters;

QLOG_INFO() << "dupa2";
    // Run G-Means
    KMeans clusterer;
    clusterer.setClusteringSet(this->instances);
    clusterer.setMaxIterations(this->maxIters);

QLOG_INFO() << "dupa3";
    while(keepGoing && iters < this->maxIters)
    {
        QLOG_INFO() << "dupa4";
        iters++;
        keepGoing = false;
        clusterer.setCentroids(initCentroids);
        // Check errors

        for(int i = 0 ; i < clusterer.getNumberOfClusters(); i++)
        {
        //    qDebug() << i;
            QList<Instance> currClusterInstances = clusterer.getInstancesForCentroid(i);

            if (currClusterInstances.isEmpty())
            {
                continue;
            }

            KMeans insideClusterer;
            insideClusterer.setClusteringSet(&currClusterInstances);
            insideClusterer.setNumberOfClusters(2);

            //if(!insideClusterer.classifyParallel())
                //return false;

            Instance centroid_1 = insideClusterer.getCentroids()->at(0);
            Instance centroid_2 = insideClusterer.getCentroids()->at(1);

            double vectorVLength = 0;
            // Centroid_1 is the V vector
            for(int j = 0 ; j < numOfAttributes; j++)
            {
                centroid_1[j] -= centroid_2[j];
                vectorVLength += centroid_1[j]*centroid_1[j];
            }

            vectorVLength = sqrt(vectorVLength);

            QList<double> setX;
            double setXMean = 0;
            foreach(Instance currInstance, currClusterInstances)
            {
                double x = 0;
                for(int j = 0; j < numOfAttributes; j++)
                {
                    x += currInstance[j] * centroid_1[j];
                }

                x /= vectorVLength;
                setXMean += x;
                setX.append(x);
            }

            double setXSize = setX.size();
            setXMean /= setXSize;

            double setXstd = 0;

            foreach(double x, setX)
            {
                setXstd += (x - setXMean)*(x - setXMean);
            }

            setXstd = sqrt(setXstd/(setXSize - 1));

            for(int j = 0 ; j < setX.count(); j++)
            {
                setX[j] = (setX[j] - setXMean)/setXstd;
            }
            // cool sort
            qSort( setX );

            double A = 0;
            for(int j = 0; j < setXSize; j++)
            {
                double CDF = GMeans::getNormalCDF(setX.at(j));
                A += (2*j-1)*log(CDF) + (2*(setXSize-j) + 1)*log(1 - CDF);
            }

            A = (-setXSize - A/setXSize)*(1 + 4/setXSize + 25/(setXSize*setXSize));

            //Accept new Cluster Centroids
            if (A > 0.787)
            {
                keepGoing = true;

                initCentroids->removeAt(i);
                initCentroids->insert(i,centroid_2);
                initCentroids->append(insideClusterer.getCentroids()->at(0));
                currentNoOfClusters++;
            }
          //  qDebug() << iters << "." << i << "A: " << A;
        }

//        if (!keepGoing)
//            qDebug() << "Keep Going Stopped Algorithm";

//        if (iters >= this->maxIters)
//            qDebug() << "Stopped because of iters limit" << iters << this->maxIters;
    }

    this->numOfClusters = currentNoOfClusters;
 //   qDebug() << initCentroids->count();
    this->itersPerformed = iters;
    QList<Instance>* finalCentroids = new QList<Instance>();

    finalCentroids->append(*initCentroids);

    this->centroids = finalCentroids;
    this->handleArtifacts();
    return true;
}

bool GMeans::classifyParallel()
{
    if (this->instances == NULL)
        return false;

    this->errMsg = "NOT IMPLEMENTED YET";
    return false;
}

bool GMeans::setClusterNumbers(int min, int max)
{
    if (max < min)
    {
        max = 0;
        return false;
    }

    this->minNumOfClusters = min;
    this->maxNumOfClusters = max;

    return true;
}

QList<Instance> *GMeans::getClasses()
{
    return this->centroids;
}

QList<int> *GMeans::getClassMembers(int classNumber)
{
    if (classNumber < this->centroids->count())
    {
        QList<int> *toReturn = new QList<int>();
        Instance centroid = this->centroids->at(classNumber);

        int instanceNo = 0;
        foreach(Instance currInstance, *this->instances)
        {
            if (this->artifacts->contains(instanceNo))
            {
                instanceNo++;
                continue;
            }
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

int GMeans::getClassRepresentative(int classNumber)
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

QList<Instance> *GMeans::getCentroids()
{
    return this->centroids;
}

int GMeans::getNumberOfClusters()
{
    return this->numOfClusters;
}

double GMeans::getNormalCDF(double x)
{
    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + CDF_P*x);
    double y = 1.0 - (((((CDF_A_5*t + CDF_A_4)*t) + CDF_A_3)*t + CDF_A_2)*t + CDF_A_1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}
