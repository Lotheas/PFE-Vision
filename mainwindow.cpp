#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //camera = new camera_support::camera_support();

    //Paramètrage du label pour l'image
    ui->lbl_img->setBackgroundRole(QPalette::Base);
    ui->lbl_img->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->lbl_img->setScaledContents(true);

    ui->btn_video->setVisible(false);
    ui->btn_stop->setVisible(false);
    ui->lbl_fps->setVisible(false);
    ui->lcd_fps->setVisible(false);

    //Pour utiliser une boucle for pour toggle leur état
    radioRes[0] = ui->radioBtn_3088x2064;
    radioRes[1] = ui->radioBtn_2316x1548;
    radioRes[2] = ui->radioBtn_1544x1032;
    radioRes[3] = ui->radioBtn_772x516;

    //Button connections
    QObject::connect(ui->btn_init, SIGNAL(clicked()), this, SLOT(onInitClick()));
    QObject::connect(ui->btn_single, SIGNAL(clicked()), this, SLOT(onSingleClick()));
    QObject::connect(ui->btn_video, SIGNAL(clicked()), this, SLOT(onVideoClick()));
    QObject::connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(onStopClick()));
    QObject::connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(onDiscoClick()));
    //Radio Button
    QObject::connect(ui->radioBtn_3088x2064, SIGNAL(clicked()), this, SLOT(onResButtonChange0()));
    QObject::connect(ui->radioBtn_2316x1548, SIGNAL(clicked()), this, SLOT(onResButtonChange1()));
    QObject::connect(ui->radioBtn_1544x1032, SIGNAL(clicked()), this, SLOT(onResButtonChange2()));
    QObject::connect(ui->radioBtn_772x516, SIGNAL(clicked()), this, SLOT(onResButtonChange3()));

    //Threshold
    cvThreshold = 0;
    cvRatio = 3;
    QObject::connect(ui->slider_ts, &QSlider::valueChanged, this, &MainWindow::onSliderChange);
    QObject::connect(ui->slider_ratio, &QSlider::valueChanged, this, &MainWindow::onSliderChange2);

    cvImage = new cv::Mat();
}

MainWindow::~MainWindow()
{
    delete cvImage;
    delete ui;
}

void MainWindow::onInitClick()
{
    bool oneRadioChecked = false;

    for(int i = 0; i < 4; i++)
        oneRadioChecked = oneRadioChecked || radioRes[i]->isChecked();

    if(oneRadioChecked)
    {
        camera.initilialize();

        //MAJ des bouttons
        ui->btn_init->setEnabled(false);
        ui->btn_single->setEnabled(true);
        ui->btn_video->setEnabled(true);
        ui->btn_stop->setEnabled(false);
        ui->btn_disconnect->setEnabled(true);

        //MAJ des radioButtons
        for(int i = 0; i < 4; i++)
        {
            radioRes[i]->setEnabled(false);
        }
    }
    else
    {
        QMessageBox msg;
        msg.setText("Merci de sélectionner une résolution!");
        msg.exec();
    }
}
void MainWindow::onSingleClick()
{

    qInfo() << "Function \"take_picture\" ";
    if(camera.take_picture(cvImage))
    {
        qInfo() << "Conversion de l'image au format RGB...";
        qInfo() << "\tDimensions de l'image source : " << cvImage->rows << " x " << cvImage->cols;
        qInfo() << "\tChannels l'image source : " << cvImage->channels();

        cv::Mat cvImageToPrint(cvImage->rows, cvImage->cols, CV_8UC3);

        qInfo() << "\tDimensions de l'image destination : " << cvImageToPrint.rows << " x " << cvImageToPrint.cols;
        qInfo() << "\tChannels de l'image destination : " << cvImageToPrint.channels();

        //Traitement de l'image
        if(ui->checkBox_traitement->isChecked())
        {
            cv::Mat cvImageGray, cvImageBlur;
            qInfo() << "\tTraitement de l'image...";

            cv::cvtColor( *cvImage, cvImageGray, cv::COLOR_BGRA2GRAY);
            cv::blur( cvImageGray, cvImageBlur, cv::Size(3,3));
            cv::Canny(cvImageGray, cvImageToPrint, cvThreshold, cvRatio*cvThreshold, 3);

            qInfo() << "\tConversion de l'image en QImage..." ;
            qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_Grayscale8);
        }
        else
        {
            cv::cvtColor( *cvImage, cvImageToPrint, cv::COLOR_BGR2RGB);

            qInfo() << "\tConversion de l'image en QImage..." ;
            qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_RGB888);
        }

        QPixmap pixmap(cvImageToPrint.cols, cvImageToPrint.rows);

        qInfo() << "===================================================";

        qInfo() << "Convertion de l'image en Pixmap";
        pixmap = QPixmap::fromImage(qImage);
        pixmap = pixmap.scaled(ui->lbl_img->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

        qInfo() << "MAJ du label... ";
//        ui->lbl_img->setFixedSize(pixmap.width(),pixmap.height());
        ui->lbl_img->setPixmap(pixmap);
        qInfo() << "Done. ";

//        ui->lbl_img->adjustSize();
        qInfo() << "Label size : ";
        qInfo() << "\t" << ui->lbl_img->width();
        qInfo() << "\t" << ui->lbl_img->height();
    }
    else
    {
        QMessageBox msg;
        msg.setText("Grab not succeded!");
        msg.exec();
    }

    //MAJ des bouttons
    ui->btn_stop->setEnabled(false);
}
void MainWindow::onVideoClick()
{
    QMessageBox msg;
    msg.setText("Triggered Video!");
    msg.exec();

    //MAJ des bouttons
    ui->btn_single->setEnabled(false);
    ui->btn_video->setEnabled(false);
    ui->btn_stop->setEnabled(true);
    ui->btn_disconnect->setEnabled(false);
}

void MainWindow::onStopClick()
{
    QMessageBox msg;
    msg.setText("Triggered Stop!");
    msg.exec();

    //MAJ des bouttons
    ui->btn_single->setEnabled(true);
    ui->btn_video->setEnabled(true);
    ui->btn_stop->setEnabled(false);
    ui->btn_disconnect->setEnabled(true);
}

void MainWindow::onDiscoClick()
{
    camera.unInitialize();

    //MAJ des bouttons
    ui->btn_init->setEnabled(true);
    ui->btn_single->setEnabled(false);
    ui->btn_video->setEnabled(false);
    ui->btn_stop->setEnabled(false);
    ui->btn_disconnect->setEnabled(false);

    //MAJ des radioButtons
    for(int i = 0; i < 4; i++)
    {
        radioRes[i]->setEnabled(true);
    }
}

void MainWindow::onResButtonChange0()
{
    camera.change_resolution(3088 , 2064);
}

void MainWindow::onResButtonChange1()
{
    camera.change_resolution(2316, 1548);
}

void MainWindow::onResButtonChange2()
{
    camera.change_resolution(1544, 1032);
}

void MainWindow::onResButtonChange3()
{
    camera.change_resolution(772, 516);
}

void MainWindow::onSliderChange()
{
    cvThreshold = ui->slider_ts->value();
    qInfo() << "Threshold set to " << cvThreshold;
}

void MainWindow::onSliderChange2()
{
    cvRatio = ui->slider_ratio->value();
    qInfo() << "Ratio set to " << cvRatio;
}
