#ifndef DATA_INPUT_H
#define DATA_INPUT_H

#include <QObject>

class data_input : public QObject
{
    Q_OBJECT
private:

    //identyfikacja pliku
    QString directory;  //katalog
    QString name;   //nazwa pliku

    //pierwsza elektroda
    QList<int> *ml2;
    //druga elektroda
    QList<int> *v1;
    //czas od rozpoczecia pomiaru
    //QList<QString> time;
    //id probki
    QList<int> sampleId;

    //info o pacjencie
    QString medicines;
    QString primaryName;
    QString secondaryName;
    //int     age;
    QChar   sex;
    //info o sygnale
    QString samplingFreq;
    QString signalLength;
    float   frequencyValue;

    bool        samples_loaded;  //flaga
public:
    QList<QString> time;
    int age;
    void LoadData();
    explicit data_input(QObject *parent = 0);
    void LoadAnnotations();
    void LoadNotes();
    void setDataFile();

signals:
public slots:
};

#endif // DATA_INPUT_H
