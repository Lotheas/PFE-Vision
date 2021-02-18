#ifndef PX2MM_H
#define PX2MM_H


#include <cstdio>
#include<QVector3D>


#include <opencv2/opencv.hpp>

class px2mm
{
public:
    px2mm();
    static std::vector<std::vector<QVector3D>> convertToMm(std::vector<std::vector<cv::Point>> trajIn, QVector3D centralPoint, QPoint dims, QPoint dimView);
};

#endif // PX2MM_H
