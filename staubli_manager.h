#ifndef STAUBLI_MANAGER_H
#define STAUBLI_MANAGER_H

#include "comm.h"

class staubli_manager
{
public:
    staubli_manager();

    QVector3D getCurrentPoint();

    void setTargetPoint(QVector3D pt);
    void setTargetPoint(qfloat16 x, qfloat16 y, qfloat16 z);

    void setRelativeTarget(QVector3D dPt);
    void setRelativeTarget(qfloat16 dx, qfloat16 dy, qfloat16 dz);

    void goToTarget();


private:
    comm communication;

    QVector3D currentCoord;

    QVector3D targetCoord;

};

#endif // staubli_manager_H
