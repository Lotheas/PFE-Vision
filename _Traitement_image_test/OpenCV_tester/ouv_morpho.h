#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "utilitaire.h"

#define POINT_SPACING 5

int morphological_skeleton(cv::Mat* image);
void removeLonelyPoint(cv::Mat image);
void spacePoint(cv::Mat image);
