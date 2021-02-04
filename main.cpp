#include "mainwindow.h"
#include "calibration_window.h"
#include "camera_support.h"
#include "staubli_manager.h"
#include "im_traitement.h"

#include <QApplication>

#include <opencv2/core/core.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    camera_support camera;
    camera.initilialize();

    staubli_manager staubli;

    cv::Mat circle = cv::imread("D:/Qt/Repos/untitled/Icons/circle2.jpg");

    //std::vector<std::vector<cv::Point>> pts = im_traitement().trajectory_generation(circle);

    calibration_window w2(nullptr, &camera, &staubli);
    w2.show();

    return a.exec();
}
