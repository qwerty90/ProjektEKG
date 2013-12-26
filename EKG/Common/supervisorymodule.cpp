//#include "supervisorymodule.h"
/*
SupervisoryModule::SupervisoryModule()
{
}
SupervisoryModule::SupervisoryModule(QString hash, ProcessingModule *rootModule)
{
    this->root = rootModule;
    this->store = new QHash<QString, ProcessingModule*>();
    this->AppendModule(hash, rootModule);
}

void SupervisoryModule::RunProcessing(EcgData *entity)
{
    root->NotifyOfParentOperationCompleted(entity);
}

void SupervisoryModule::AppendModule(QString hash, ProcessingModule *module)
{
    this->store->insert(hash, module);
    module->StoreConfiguration();
}

bool SupervisoryModule::RunSingle(EcgData *entity, QString hash)
{
    ProcessingModule *module = this->store->value(hash);
    if(!module)
        return false;
    bool processed = module->ProcessSingle(entity);
    return processed;
}

void SupervisoryModule::run()
{
    if(this->entity)
    {
        this->RunProcessing(this->entity);
        emit this->Finished();
    }
}

void SupervisoryModule::ResetModules()
{
    this->root->ResetModule();
}
*/
