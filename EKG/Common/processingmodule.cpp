//#include "processingmodule.h"
/*

ProcessingModule::ProcessingModule()
{
    children = new QList<ProcessingModule*>();
    _uncompletedParentOperations = 0;
    _isProcessed = false;
}

void ProcessingModule::SetNextChild(ProcessingModule *child)
{
    if(this->children)
    {
        child->IncrementParents();
        this->children->append(child);
    }
}

void ProcessingModule::NotifyOfParentOperationCompleted(EcgData *result)
{
    if(_uncompletedParentOperations <= 0)
    {
        if(_isProcessed)
        {
            if(!this->children)
                return;
            for(int i = 0; i < this->children->length(); i++)
            {
                this->children->at(i)->NotifyOfParentOperationCompleted(result);
            }
        }
        else
        {
            this->Process(result);
            this->_isProcessed = true;
            if(!this->children)
                return;
            for(int i = 0; i < this->children->length(); i++)
            {
                this->children->at(i)->DecrementDependecies();
                this->children->at(i)->NotifyOfParentOperationCompleted(result);
            }
        }
    }
}

void ProcessingModule::IncrementParents()
{
    this->_uncompletedParentOperations++;
}

void ProcessingModule::DecrementDependecies()
{
    this->_uncompletedParentOperations--;
}

bool ProcessingModule::ProcessSingle(EcgData *data)
{
    if(_uncompletedParentOperations > 0)
        return false;

    this->Process(data);
    this->_isProcessed = true;
    if(!this->children)
        return true;
    for(int i = 0; i < this->children->length(); i++)
    {
        this->children->at(i)->DecrementDependecies();
    }
    return true;
}

void ProcessingModule::StoreConfiguration()
{
    this->_parentsCount = this->_uncompletedParentOperations;
}

void ProcessingModule::ResetModule()
{
    this->_uncompletedParentOperations = this->_parentsCount;
    this->_isProcessed = false;
    if(this->children)
    {
        for(int i = 0; i < this->children->count(); i++)
        {
            this->children->at(i)->ResetModule();
        }
    }
}*/
