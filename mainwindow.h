#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QPixmap>
#include <QGraphicsView>
#include <qstring.h>
#include <QDebug>
#include "camera_support.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInitClick();
    void onSingleClick();
    void onVideoClick();
    void onStopClick();
    void onDiscoClick();

    void onResButtonChange0();
    void onResButtonChange1();
    void onResButtonChange2();
    void onResButtonChange3();

    void onSliderChange();
    void onSliderChange2();

private:
    void setRadioBtnState(bool state);
    Ui::MainWindow *ui;
    QRadioButton *radioRes[4];

    QSize sizeImage;
    QImage qImage;

    camera_support camera;

    int cvThreshold;
    int cvRatio;

    cv::Mat *cvImage;
};
#endif // MAINWINDOW_H
