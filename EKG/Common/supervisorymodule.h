#ifndef SUPERVISORYMODULE_H
#define SUPERVISORYMODULE_H

#include <QObject>
#include "Common/ecgdata.h"
#include "processingmodule.h"
#include "ecgdata.h"
#include <QHash>
#include <QThread>

class SupervisoryModule : public QThread
{
    Q_OBJECT

protected:
    void run();

private:
    ProcessingModule *root;
    QHash<QString, ProcessingModule*> *store;

public:
    explicit SupervisoryModule();
    EcgData *entity;
    SupervisoryModule(QString hash, ProcessingModule *rootModule);
    void RunProcessing(EcgData *entity);
    void AppendModule(QString hash, ProcessingModule *module);
    bool RunSingle(EcgData *entity, QString hash);
    void ResetModules();

signals:
    void Finished();
    
public slots:
    
};

#endif // SUPERVISORYMODULE_H
