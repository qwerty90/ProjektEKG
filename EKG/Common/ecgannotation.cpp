#include "ecgannotation.h"

EcgAnnotation::EcgAnnotation(QString t, int sid, QChar tp)
{
    this->time = t;
    this->sampleId = sid;
    this->type = tp;
}
