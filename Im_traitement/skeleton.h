#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>
#include <vector>

#include <opencv2/opencv.hpp>

#include "utilitaire.h"

#define POINT_SPACING 3

void morphological_skeleton(cv::Mat* image); 
cv::Mat amincissement(cv::Mat image, cv::Mat element);
cv::Mat rotate45(cv::Mat _m);
void matrixVisual(cv::Mat _m);
