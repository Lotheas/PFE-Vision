#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>
#include <opencv2/opencv.hpp>

int stentiford_process(cv::Mat image);
void stentiford_algorithm(cv::Mat image, cv::Mat* T);
