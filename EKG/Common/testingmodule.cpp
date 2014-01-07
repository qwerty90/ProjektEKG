#include "testingmodule.h"
#include <QDebug>
#include "ecgentry.h"
#include <QTime>
#include "Controllers/appcontroller.h"

TestingModule::TestingModule(QObject *parent) :
    QObject(parent)
{
}

void TestingModule::RunTest(){
    qDebug() << "Test started";
    //this->SignalsTest();
    //this->SignalInfoTest();
    //this->PhysicalSamplesTest();
    //this->SpeedTest();
    this->AppEngineTest();
    qDebug() << "Test finished";
}

void TestingModule::SignalsTest()
{
    EcgEntry *entry = new EcgEntry();
    QStringList list = entry->GetEcgList("..\\ECG_APP\\Data");
    int c = list.count();
    for(int i = 0; i < c; i++)
    {
        QString *resp = new QString("");
        qDebug() << "Opening " + list.at(i);
        bool correct = entry->Open("..\\ECG_APP\\Data", list.at(i), *resp, false);
        if(correct)
        {
            resp = new QString("Opened correctly");
            qDebug() << *resp;
        }
        else
        {
            qDebug() << *resp;
            continue;
        }

        qDebug() << "MLII_values";
        int min = entry->entity->MLII_values->first();
        int max = min;
        int current;

        for(int j = 0; j < entry->entity->MLII_values->length(); j++)
        {
            current = entry->entity->MLII_values->at(j);
            if(current > max)
            {
                max = current;
            }
            if(current < min)
            {
                min = current;
            }
        }

        QString *s1 = new QString("min: ");
        s1->append(s1->number(min));
        QString *s2 = new QString("max: ");
        s2->append(s2->number(max));

        qDebug() << *s1;
        qDebug() << *s2;

        qDebug() << "VI_values";
        min = entry->entity->VI_values->first();
        max = min;

        for(int j = 0; j < entry->entity->VI_values->length(); j++)
        {
            current = entry->entity->VI_values->at(j);
            if(current > max)
            {
                max = current;
            }
            if(current < min)
            {
                min = current;
            }
        }

        s1 = new QString("min: ");
        s1->append(s1->number(min));
        s2 = new QString("max: ");
        s2->append(s2->number(max));

        qDebug() << *s1;
        qDebug() << *s2;
    }
}

void TestingModule::SignalInfoTest()
{
    EcgEntry *entry = new EcgEntry();
    QStringList list = entry->GetEcgList("Data");
    for(int i = 0; i < list.count(); i++)
    {
        QString *resp = new QString("");
        qDebug() << "Opening " + list.at(i);
        bool correct = entry->Open("Data", list.at(i), *resp, false);
        if(correct)
        {
            resp = new QString("Opened correctly");
            qDebug() << *resp;
        }
        else
        {
            qDebug() << *resp;
            continue;
        }

        QString *pdata = new QString("Sex: ");
        pdata->append(entry->entity->info->sex);
        pdata->append(" Age: ");
        pdata->append(pdata->number(entry->entity->info->age));

        qDebug() << *pdata;

        QString *sdata = new QString("Sig length: ");
        sdata->append(entry->entity->info->signalLength);
        sdata->append(" Sampling frequency: ");
        sdata->append(entry->entity->info->samplingFreq);

        qDebug() << *sdata;
    }
}

void TestingModule::SpeedTest()
{
    QTime *timer = new QTime();
    timer->start();
    EcgEntry *entry = new EcgEntry();
    QStringList list = entry->GetEcgList("Data");
    int i;
    for(i = 0; i < list.count(); i++)
    {
        QString *resp = new QString("");
        qDebug() << "Opening " + list.at(i);
        bool correct = entry->Open("Data", list.at(i), *resp, false);
        if(correct)
        {
            resp = new QString("Opened correctly");
            qDebug() << *resp;
        }
        else
        {
            qDebug() << *resp;
            continue;
        }
    }

    QString *str = new QString("Total time: ");
    str->append(str->number(timer->elapsed()));
    qDebug() << *str;
    str = new QString("Time per record: ");
    str->append(str->number(timer->elapsed()/i));
    qDebug() << *str;
}

void TestingModule::AppEngineTest()
{
//    AppController *controller = new AppController();
//    controller->RunMock();
}
