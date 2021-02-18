#include "px2mm.h"

using namespace std;

px2mm::px2mm()
{

}

std::vector<std::vector<QVector3D>> px2mm::convertToMm(std::vector<std::vector<cv::Point>> trajIn, QVector3D centralPoint, QPoint dims, QPoint dimView)
{

    std::vector<std::vector<QVector3D>> trajOut;

    for (uint i = 0; i < trajIn.size(); i++)
    {
        std::vector<QVector3D> newIn;


        for (uint j = 0; j < trajIn[i].size(); j++)
        {
            QVector3D vecOut;

            //Hauteur constante par rapport au 0
            vecOut.setZ(centralPoint.z());

            vecOut.setX((trajIn[i][j].y - dims.y() / 2) * (dimView.y() / 2) / (dims.y() / 2) + centralPoint.x());
            vecOut.setY((trajIn[i][j].x - dims.x() / 2) * (dimView.x() / 2) / (dims.x() / 2) + centralPoint.y());

            newIn.push_back(vecOut);

        }

        trajOut.push_back(newIn);
    }

    return trajOut;
}


