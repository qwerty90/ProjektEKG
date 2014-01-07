#ifndef PROCESSINGMODULE_H
#define PROCESSINGMODULE_H

#include "ecgdata.h"

/*
 * Abstrakcyjna klasa podstawowa dla modulow obliczeniowych
 */

class ProcessingModule
{

public:
    ProcessingModule();

    void virtual Process(EcgData *data) = 0;
    void SetNextChild(ProcessingModule* child);
    void NotifyOfParentOperationCompleted(EcgData *result);
    void IncrementParents();
    bool ProcessSingle(EcgData *data);
    void DecrementDependecies();
    void StoreConfiguration();
    void ResetModule();

protected:
    QList<ProcessingModule*> *children;
    int _uncompletedParentOperations;
    int _parentsCount;
    bool _isProcessed;
signals:
    
public slots:
    
};

#endif // PROCESSINGMODULE_H
