#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>

#define SEPARATOR_LENGTH 120

void printSeparator(std::string title);
void printVoidSeparator(char character);
void showCurrentPoint(cv::Mat skeleton, cv::Point pt);