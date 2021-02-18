#ifndef IM_TRAITEMENT_H
#define IM_TRAITEMENT_H

#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>

#include <opencv2/opencv.hpp>

#include <QObject>

#include "Im_traitement\traj_gene.h"
#include "Im_traitement\skeleton.h"
#include "Im_traitement\utilitaire.h"

#define SCALE 2
#define DOWNSCALE true

class im_traitement //: public QObject
{
    //Q_OBJECT

public:
    im_traitement();
    std::vector<std::vector<cv::Point>> trajectory_generation(cv::Mat image);

//signals:
//    void stepChanged(int value);
};

#endif // IM_TRAITEMENT_H
