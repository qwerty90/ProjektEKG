#include "instance.h"
#include <iostream>

using namespace std;

Instance::Instance()
    : numOfAttributes(1)
{
    this->attributes = new double[this->numberOfAttributes()];
    for (int i = 0; i < numOfAttributes ; i++)
        this->attributes[i] = 2;
}

Instance::Instance(int numOfAttributes)
    : numOfAttributes(numOfAttributes)
{
    this->attributes = new double[numOfAttributes];

    for (int i = 0; i < numOfAttributes ; i++)
        this->attributes[i] = 0;
}

Instance::Instance(const Instance &object)
    : numOfAttributes(object.numOfAttributes)
{
    this->attributes = new double[numOfAttributes];

    for(int i = 0 ; i < numOfAttributes ; i++)
    {
        this->attributes[i] = double(object.attributes[i]);
    }
}

Instance::~Instance()
{
    delete this->attributes;
}

int Instance::numberOfAttributes() const
{
    return numOfAttributes;
}

double& Instance::operator [](int index)
{
    return this->attributes[index];
}

Instance &Instance::operator =( Instance object)
{
    delete this->attributes;
    this->numOfAttributes = object.numOfAttributes;
    this->attributes = new double[numOfAttributes];

    for(int i = 0 ; i < numOfAttributes ; i++)
    {
        this->attributes[i] = double(object.attributes[i]);
    }

    return *this;
}
