#pragma once

#include <iostream>
#include <cstdio>
#include <ctime>

#include <opencv2/opencv.hpp>

int zhang_suen_process(cv::Mat image);
void zhang_suen_algorithm(cv::Mat image);
bool conditions3puis4(cv::Mat neighbourhood);
int blackNeighbour(cv::Mat neighbourhood);
int connectivites(cv::Mat neighbourhood);
