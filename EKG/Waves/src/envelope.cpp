#include <iostream>
#include <cmath>
#include <fstream>
#include <QVector>
#include <stdlib.h>

using namespace std;

void envelope(QVector<double>::iterator signal_start, QVector<double>::iterator signal_end, QVector<double>::iterator hilbert_start, QVector<double>::iterator hilbert_end)
{
    for(int i=0; i<signal_end-signal_start;i++)
    {
        *(signal_start+i)=sqrt(pow(*(signal_start+i),2)+pow(*(hilbert_start+i),2));
    }
}
