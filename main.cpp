#include "mainwindow.h"


#include <QApplication>

#include <opencv2/core/core.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    //std::vector<std::vector<cv::Point>> pts = im_traitement().trajectory_generation(circle);

    return a.exec();
}
