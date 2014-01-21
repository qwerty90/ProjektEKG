#ifndef INSTANCE_H
#define INSTANCE_H

class Instance
{
    double* attributes;
    int numOfAttributes;
public:
    Instance();
    Instance(int numOfAttributes);
    Instance(const Instance &object);
    ~Instance();
    int numberOfAttributes() const;
    double& operator[] (int index);
    Instance& operator=(Instance object);
};

#endif // INSTANCE_H
