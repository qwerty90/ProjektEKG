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

public:
    //pierwsza elektroda
    QList<int> *ml2;
    //druga elektroda
    QList<int> *v1;
    //czas od rozpoczecia pomiaru
    QList<QString> time;
    //id probki
    QList<int> sampleId;
private:

    //info o pacjencie
    QString medicines;
    QString primaryName;
    QString secondaryName;
    int     age;
    QChar   sex;

    //info o sygnale
    QString samplingFreq;
    QString signalLength;
    float   frequencyValue;

    bool    samples_loaded;         //flaga
    bool    annotations_loaded;     //flaga
    bool    notes_loaded;           //flaga
    bool    data_valid;
    int     samples_count;
public:

    explicit data_input(QObject *parent = 0);
    void LoadSamples();
    void LoadAnnotations();
    void LoadNotes();
    void setDataFile();
    void validate_data_input();
    bool LoadAllData(QString name, QString dir);

    bool check_valid();

signals:
public slots:
};

#endif // DATA_INPUT_H
