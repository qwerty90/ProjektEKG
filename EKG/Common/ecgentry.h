#ifndef ECGENTRY_H
#define ECGENTRY_H

#include <QObject>
#include <QFile>
#include "ecgdata.h"
#include <QStringList>

class EcgEntry : public QObject
{
    Q_OBJECT
public:
    EcgData *entity;

    explicit EcgEntry(QObject *parent = 0);
    bool Open(QString directory, QString record, QString &response);
    QStringList GetEcgList(QString directory);

private:

    QString* Validate(QFile *file);
    void LoadSamples(QString name);
    void LoadPhysicalValues(QFile *file);
    void LoadAnnotations(QFile *file);
    void LoadNotes(QFile *file);
    
signals:
    
public slots:
    
};

#endif // ECGENTRY_H
