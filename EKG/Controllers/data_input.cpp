#include "data_input.h"

#include <QVector>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QStringList>

data_input::data_input(QObject *parent) :
    QObject(parent)
{
}

void data_input::LoadSamples()
{
    //Load Samples
    QList<int> *column1 = new QList<int>();
    QList<int> *column2 = new QList<int>();

    QVector<int> data;// = new QVector<int>;
    QFile f(this->directory + "\\" + this->name + ".dat");
    f.open(QIODevice::ReadOnly);

    if (!f.exists())
    {
        this->samples_loaded = false;
        return;
    }

    QDataStream in(&f);
    in >> data;

    f.close();
    for(int i = 0; i < data.count(); i+=2)
    {
        column1->append(data.at(i));
        column2->append(data.at(i + 1));
    }

    this->ml2 = column1;
    this->v1  = column2;
    this->samples_loaded = 1;
}

void data_input::LoadAnnotations()
{
    QFile f(this->directory + "\\" + this->name + "c.txt");
    f.open(QIODevice::ReadOnly);
    if (!f.exists())
    {
        this->annotations_loaded = false;
        return;
    }

    QTextStream *in = new QTextStream(&f);
    QString line = in->readLine();
    bool stop = in->atEnd();

    while(!stop)
    {
        QStringList elements = line.split(" ");
        elements.removeAll("");
        this->time.push_back(elements.at(0));
        this->sampleId.push_back(elements.at(1).toInt());

        line = in->readLine();
        stop = in->atEnd();
    }
//jeszcze jedna probka powinna byc
    QStringList elements = line.split("   ");
   // elements.removeAll("");
    this->time.push_back(elements.at(0));
    this->sampleId.push_back(elements.at(1).toInt());




    f.close();
    this->annotations_loaded = true;
}

void data_input::LoadNotes()
{
    QFile f(this->directory + "\\" + this->name + "d.txt");
    f.open(QIODevice::ReadOnly);
    if (!f.exists())
    {
        this->notes_loaded = false;
        return;
    }

    QTextStream *in = new QTextStream(&f);
    QStringList *list = new QStringList();
    QString line = in->readLine();

    bool stop = in->atEnd();
    while(!stop)
    {
        list->append(line);
        line = in->readLine();
        stop = in->atEnd();
    }

    f.close();

    this->medicines = list->at(4);

    QRegExp *sfre = new QRegExp("Sampling frequency:");
    QStringList sf = list->filter(*sfre).first().split(" ");
    this->samplingFreq =
            sf.at(sf.length()-2) + " " + sf.at(sf.length()-1);

    QRegExp *slre = new QRegExp("Length:");
    QStringList sl = list->filter(*slre).first().split(" ");
    sl.removeAll("");
    this->signalLength = sl.at(1);

    QRegExp *dlre = new QRegExp("Description:");
    QStringList sd = list->filter(*dlre).first().split(" ");
    sd.removeAll("");
    this->primaryName = sd.at(1);
    sd = list->filter(*dlre).last().split(" ");
    sd.removeAll("");
    this->secondaryName = sd.at(1);

    QString patient = list->at(3);
    QStringList pinfo = patient.split(" ");

    this->age = pinfo.at(1).toInt();
    this->sex = pinfo.at(2).at(0);

    int n = this->samplingFreq.length();
    QString value = this->samplingFreq.mid(0, n-2);
    this->frequencyValue = value.toFloat();

    this->notes_loaded=true;

}

void data_input::setDataFile()
{
    //to trzeba będzie wczytywać!!!!!!
    this->directory="C:\\Qt\\workspace\\EKG\\Data";
    this->name="100";
}

void data_input::validate_data_input()
{
    this->data_valid=true;

    this->samples_count=this->time.count();
//    if (this->samples_count != this->sampleId.count())
//        this->data_valid=false;
//    if (this->samples_count != this->ml2->count())
//        this->data_valid=false;
//    if (this->samples_count != this->v1->count())
//        this->data_valid=false;
}

bool data_input::check_valid()
{
    return this->data_valid;
}

bool data_input::LoadAllData(QString name, QString dir)
{

    this->setDataFile();
    this->LoadSamples();
    this->LoadAnnotations();
    this->LoadNotes();
    this->validate_data_input();

    return this->data_valid;
}
