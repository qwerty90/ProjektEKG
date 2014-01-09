#ifndef ECGSTDESCRIPTOR_H
#define ECGSTDESCRIPTOR_H

//------------------------------------------------------------

#include <QVector>

//------------------------------------------------------------

enum EcgStPosition {
    ST_POS_NORMAL,
    ST_POS_ELEVATION,
    ST_POS_DEPRESSION
};

enum EcgStShape {
    ST_SHAPE_HORIZONTAL,
    ST_SHAPE_DOWNSLOPING,
    ST_SHAPE_UPSLOPING,
    ST_SHAPE_CONCAVE,
    ST_SHAPE_CONVEX
};

//------------------------------------------------------------

struct EcgStDescriptor
{
    QVector<double>::const_iterator STOn;
    QVector<double>::const_iterator STEnd;
    QVector<double>::const_iterator STMid;

    double offset;
    double slope1;
    double slope2;

    EcgStPosition position;
    EcgStShape shape;
};

//------------------------------------------------------------

#endif // ECGSTDESCRIPTOR_H
