#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "utilitaire.h"
#include "ouv_morpho.h"

void trajectoryGeneration(cv::Mat image, cv::Mat skeleton, std::vector<std::vector<cv::Point>>* trajectories);
int getRegion(cv::Mat img, cv::Mat* region);
void pointsInRegion(cv::Mat skeleton, cv::Mat region, cv::Mat result);
void trajectoryInRegion(cv::Mat skeleton, int regionNbr, cv::Mat pointsDone, std::vector<std::vector<cv::Point>>* trajectories);
void suppressionVoisinnage(cv::Mat *skeleton, cv::Point point, int* ptNbr, int dist);


