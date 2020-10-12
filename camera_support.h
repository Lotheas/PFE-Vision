#ifndef CAMERA_SUPPORT_H
#define CAMERA_SUPPORT_H

#include <QMessageBox>
#include<QDebug>

#include "pylon/PylonIncludes.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

class camera_support
{    
public:
    camera_support();
    ~camera_support();
    void initilialize();
    bool take_picture(cv::Mat *result);
    void change_resolution(uint width, uint height);
    void unInitialize();

private:
    Pylon::CInstantCamera *camera;

    cv::Mat cvImage;

    uint im_width, im_height;
};

#endif // CAMERA_SUPPORT_H
