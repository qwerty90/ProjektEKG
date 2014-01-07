#include "ecgentry.h"
#include <QTextStream>
#include <QStringList>
#include <QDir>
#include <QRegExp>
#include <QList>
#include "ecgannotation.h"
#include "ecginfo.h"
#include <QVector>
#include <QDataStream>

EcgEntry::EcgEntry(QObject *parent) :
    QObject(parent)
{
    this->entity = new EcgData();
}

QString* EcgEntry::Validate(QFile *file)
{
    if(!file->exists())
    {
        return new QString("The file does not exists!");
    }
    if(!file->open(QIODevice::ReadOnly)) {
        return new QString("Unable to open this file!");
    }
    return NULL;
}

void EcgEntry::LoadSamples(QString name)
{
    QList<int> *ml2 = new QList<int>();
    QList<int> *v1 = new QList<int>();

    QVector<int> data;
    QFile f(name);
    f.open(QIODevice::ReadOnly);
    QDataStream in(&f);
    in >> data;

    f.close();
    for(int i = 0; i < data.count(); i+=2)
    {
        ml2->append(data.at(i));
        v1->append(data.at(i + 1));
    }

    this->entity->primary = ml2;
    this->entity->secondary = v1;
    this->entity->settings->signalIndex = 1;
}

void EcgEntry::LoadAnnotations(QFile *file)
{
    this->entity->annotations = new QList<EcgAnnotation>();

    QTextStream *in = new QTextStream(file);
    QString line = in->readLine();
    bool stop = in->atEnd();

    while(!stop)
    {
        QStringList elements = line.split(" ");
        elements.removeAll("");
        EcgAnnotation annotation(elements.at(0),
                                  elements.at(1).toInt(),
                                  elements.at(2).at(0));
        annotation.sub = elements.at(3).toInt();
        annotation.chan = elements.at(4).toInt();
        QString temp = elements.at(5);
        if(temp.length() == 1)
        {
            annotation.num = temp.toInt();
        }
        else
        {
            QString s(temp.at(0));
            annotation.num = s.toInt();
            annotation.aux = temp.mid(2);
        }
        EcgAnnotation* ptr = &annotation;

        this->entity->annotations->append(*ptr);
        line = in->readLine();
        stop = in->atEnd();
    }

    file->close();
}

void EcgEntry::LoadNotes(QFile *file)
{
    this->entity->info = new EcgInfo();

    QTextStream *in = new QTextStream(file);
    QStringList *list = new QStringList();
    QString line = in->readLine();

    bool stop = in->atEnd();
    while(!stop)
    {
        list->append(line);
        line = in->readLine();
        stop = in->atEnd();
    }

    file->close();

    this->entity->info->medicines = list->at(4);

    QRegExp *sfre = new QRegExp("Sampling frequency:");
    QStringList sf = list->filter(*sfre).first().split(" ");
    this->entity->info->samplingFreq =
            sf.at(sf.length()-2) + " " + sf.at(sf.length()-1);

    QRegExp *slre = new QRegExp("Length:");
    QStringList sl = list->filter(*slre).first().split(" ");
    sl.removeAll("");
    this->entity->info->signalLength = sl.at(1);

    QRegExp *dlre = new QRegExp("Description:");
    QStringList sd = list->filter(*dlre).first().split(" ");
    sd.removeAll("");
    this->entity->info->primaryName = sd.at(1);
    sd = list->filter(*dlre).last().split(" ");
    sd.removeAll("");
    this->entity->info->secondaryName = sd.at(1);

    QString patient = list->at(3);
    QStringList pinfo = patient.split(" ");

    this->entity->info->age = pinfo.at(1).toInt();
    this->entity->info->sex = pinfo.at(2).at(0);
    this->entity->info->parseFrequency();
}

bool EcgEntry::Open(QString directory, QString record, QString &response)
{

    this->entity = new EcgData(record);
    QString *notValid;

    this->LoadSamples(directory + "\\" + record + ".dat");

    QFile *file;
    file = new QFile(directory + "\\" + record + "c.txt");
    notValid = this->Validate(file);
    if(notValid)
    {
        response = *notValid;
        file->close();
        return false;
    }

    this->LoadAnnotations(file);

    file = new QFile(directory + "\\" + record + "d.txt");
    notValid = this->Validate(file);
    if(notValid)
    {
        response = *notValid;
        file->close();
        return false;
    }

    this->LoadNotes(file);

    return true;
}


QStringList EcgEntry::GetEcgList(QString directory)
{
    QDir myDir(directory);
    QRegExp *are = new QRegExp(".dat");
    QRegExp *cre = new QRegExp("c.txt");
    QRegExp *dre = new QRegExp("d.txt");

    QStringList datafiles = myDir.entryList().filter(*are);
    QStringList *records = new QStringList();
    QStringList fa, fb, fc, fd;

    //sprawdzenie czy wszystkie 3 pliki z danymi sa w folderze

    fc = myDir.entryList().filter(*cre);
    fd = myDir.entryList().filter(*dre);

    for(int i = 0; i < datafiles.count(); i++)
    {
        QString index = datafiles.at(i).mid(0,3);
        if(fc.contains(index + "c.txt") && fd.contains(index + "d.txt"))
        {
            records->append(index);
        }
    }

    return *records;

}
