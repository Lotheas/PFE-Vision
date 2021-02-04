#ifndef IM_TRAITEMENT_H
#define IM_TRAITEMENT_H

#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>

#include <opencv2/opencv.hpp>

#include "Im_traitement\traj_gene.h"
#include "Im_traitement\skeleton.h"
#include "Im_traitement\utilitaire.h"

#define SCALE 3
#define DOWNSCALE true

class im_traitement
{
public:
    im_traitement();
    static std::vector<std::vector<cv::Point>> im_traitement::trajectory_generation(cv::Mat image);
};

#endif // IM_TRAITEMENT_H
