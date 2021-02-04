#include "staubli_manager.h"

staubli_manager::staubli_manager()
{
    currentCoord.setX(400);
    currentCoord.setY(-100);
    currentCoord.setZ(157);

    targetCoord = currentCoord;
}

QVector3D staubli_manager::getCurrentPoint()
{
    return currentCoord;
}

void staubli_manager::setTargetPoint(QVector3D pt)
{
    targetCoord = pt;
}

void staubli_manager::setTargetPoint(qfloat16 x, qfloat16 y, qfloat16 z)
{
    targetCoord.setX(x);
    targetCoord.setY(y);
    targetCoord.setZ(z);
}

void staubli_manager::setRelativeTarget(QVector3D dPt)
{
    targetCoord.setX(targetCoord.x() + dPt.x());
    targetCoord.setY(targetCoord.y() + dPt.y());
    targetCoord.setZ(targetCoord.z() + dPt.z());
}

void staubli_manager::setRelativeTarget(qfloat16 dx, qfloat16 dy, qfloat16 dz)
{
    targetCoord.setX(targetCoord.x() + dx);
    targetCoord.setY(targetCoord.y() + dy);
    targetCoord.setZ(targetCoord.z() + dz);
}

void staubli_manager::goToTarget()
{
    currentCoord.setX(targetCoord.x());
    currentCoord.setY(targetCoord.y());
    currentCoord.setZ(targetCoord.z());

    communication.envoie(currentCoord.x(),  currentCoord.y(), currentCoord.z());

    qInfo() << "Send coord : " << "currentCoord." << currentCoord.x() << " : " << currentCoord.y() << " : " << currentCoord.z();
}

bool staubli_manager::online()
{
    if(communication.connected())
        return true;
    else
        return false;

}
