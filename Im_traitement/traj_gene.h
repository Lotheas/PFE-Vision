#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "utilitaire.h"
#include "skeleton.h"

typedef struct { cv::Point pt; std::vector<cv::Point> voisins; } ptClef;

void trajectoryGeneration( cv::Mat skeleton, bool ebranchage, std::vector<std::vector<cv::Point>>* trajectories);
int getRegion(cv::Mat img, cv::Mat* region);
void pointsInRegion(cv::Mat skeleton, cv::Mat region, cv::Mat result);
void ebrancher(cv::Mat image);
void trajectoryInRegion(cv::Mat skeleton, std::vector<std::vector<cv::Point>>* trajectories);
bool isKeyPointOrNear(cv::Point pt, std::vector<ptClef> ptsClefs);
void removeNearOfKeyPoint(cv::Point pt, std::vector<ptClef>* ptsClefs);


