#pragma once

#include <iostream>
#include <Windows.h>
#include <time.h> 

#include <opencv2/opencv.hpp>

#define SEPARATOR_LENGTH 120

void printSeparator(std::string title);
void printVoidSeparator(char character);
void showCurrentPoint(cv::Mat skeleton, cv::Point pt, uint scale);
void drawTrajectories(cv::Mat skeleton, std::vector<std::vector<cv::Point>> trajectories, uint pointPerFrame, uint scale);
void fusionTrajectories(std::vector<std::vector<cv::Point>> trajIn, std::vector<std::vector<cv::Point>>* trajOut, uint distanceMax);
void lightenTrajectories(std::vector<std::vector<cv::Point>> trajIn, std::vector<std::vector<cv::Point>>* trajOut, uint spacing);