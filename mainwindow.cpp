#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    w2 = new calibration_window (nullptr, &camera, &staubli);

    connect(w2, &calibration_window::offsetsChanged, this, &MainWindow::onOffsetChange);

    offsets.setX(0);
    offsets.setY(0);
    offsets.setZ(0);

    //Paramètrage du label pour l'image
    ui->lbl_img->setBackgroundRole(QPalette::Base);
    ui->lbl_img->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->lbl_img->setScaledContents(true);

    //Pour utiliser une boucle for pour toggle leur état
    radioRes[0] = ui->radioBtn_3088x2064;
    radioRes[1] = ui->radioBtn_2316x1548;
    radioRes[2] = ui->radioBtn_1544x1032;
    radioRes[3] = ui->radioBtn_772x516;

    //Button connections
    QObject::connect(ui->btn_init, SIGNAL(clicked()), this, SLOT(onInitClick()));
    QObject::connect(ui->btn_single, SIGNAL(clicked()), this, SLOT(onSingleClick()));
    QObject::connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(onDiscoClick()));
    //Radio Button
    QObject::connect(ui->radioBtn_3088x2064, SIGNAL(clicked()), this, SLOT(onResButtonChange0()));
    QObject::connect(ui->radioBtn_2316x1548, SIGNAL(clicked()), this, SLOT(onResButtonChange1()));
    QObject::connect(ui->radioBtn_1544x1032, SIGNAL(clicked()), this, SLOT(onResButtonChange2()));
    QObject::connect(ui->radioBtn_772x516, SIGNAL(clicked()), this, SLOT(onResButtonChange3()));

    //CheckBox Traitement image
    QObject::connect(ui->checkBox_seuillage, SIGNAL(stateChanged(int)), this, SLOT(onCkBxSeuillageChange(int)));

    //Threshold traitement image
    cvThreshold = 0;
    cvRatio = 3;
    QObject::connect(ui->slider_ts, &QSlider::valueChanged, this, &MainWindow::onSliderChange);
    QObject::connect(ui->slider_ratio, &QSlider::valueChanged, this, &MainWindow::onSliderChange2);

    //Allocation dynamique de cvImage; devra être "delete"
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
        ui->btn_disconnect->setEnabled(true);

        //MAJ des radioButtons
        for(int i = 0; i < 4; i++)
        {
            radioRes[i]->setEnabled(false);
        }

        w2->setAttribute(Qt::WA_DeleteOnClose);
        w2->show();

        // This loop will wait for the window is destroyed
        QEventLoop loop;
        connect(w2, SIGNAL(destroyed()), & loop, SLOT(quit()));
        loop.exec();

        qInfo() << "Offsets from main : " << offsets.x() << " : " << offsets.y() << " : " << offsets.z();
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
        qInfo() << "\tDimensions de l'image source : " << cvImage->rows << " x " << cvImage->cols;
        qInfo() << "\tChannels l'image source : " << cvImage->channels();

        imageTraitement();

        QPixmap pixmap(qImage.width(), qImage.height());

        qInfo() << "===================================================";

        qInfo() << "Convertion de l'image en Pixmap";
        pixmap = QPixmap::fromImage(qImage);
        pixmap = pixmap.scaled(ui->lbl_img->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

        qInfo() << "MAJ du label... ";
        ui->lbl_img->setPixmap(pixmap);
        qInfo() << "Done. ";

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

}


void MainWindow::onDiscoClick()
{
    //Déconnect la caméra
    camera.unInitialize();

    //MAJ des bouttons
    ui->btn_init->setEnabled(true);
    ui->btn_single->setEnabled(false);
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

void MainWindow::onCkBxContoursChange(int state)
{
    if(state == Qt::Checked)
    {
        typeDeTraitement = (enumTypeDeTraitement)(typeDeTraitement | contours);
        qInfo() << "Type de traitement : contours activés. \t\tNouveau flag : \t" << typeDeTraitement;
    }
    else
    {
        typeDeTraitement = (enumTypeDeTraitement)(typeDeTraitement & ~contours);
        qInfo() << "Type de traitement : contours désactivés. \tNouveau flag : \t" << typeDeTraitement;
    }
}

void MainWindow::onCkBxSeuillageChange(int state)
{
    if(state == Qt::Checked)
    {
        typeDeTraitement = (enumTypeDeTraitement)(typeDeTraitement | seuillage);
        qInfo() << "Type de traitement : seuillage activés. \t\tNouveau flag : \t" << typeDeTraitement;
    }
    else
    {
        typeDeTraitement = (enumTypeDeTraitement)(typeDeTraitement & ~seuillage);
        qInfo() << "Type de traitement : seuillage désactivés. \tNouveau flag : \t" << typeDeTraitement;
    }
}

void MainWindow::onOffsetChange(QVector3D offs)
{
    offsets = offs;
}

/***************************************************************
 *                      TRAITEMENT DE L'IMAGE
 * ************************************************************/
void MainWindow::imageTraitement()
{
    qInfo() << "Analyse de l'image : ";

    //Déclaration de l'image qui sera ensuite affichée dans l'UI
    cvImageToPrint.create(cvImage->rows, cvImage->cols, CV_8UC3);

    //qInfo() << "\tDimensions de l'image destination : " << cvImageToPrint.rows << " x " << cvImageToPrint.cols;
    //qInfo() << "\tChannels de l'image destination : " << cvImageToPrint.channels();

    if(typeDeTraitement & seuillage)
    {
        //qInfo() << "\tDétection de contours...";

        cv::Mat cvImageGray, cvImageBlur;
        qInfo() << "\tSeuillage de l'image...";

        if(cvImage->type() != CV_8U)
            cv::cvtColor( *cvImage, cvImageGray, cv::COLOR_BGRA2GRAY);
        cv::blur( cvImageGray, cvImageBlur, cv::Size(3,3));
        cv::threshold(cvImageBlur,cvImageToPrint, ui->slider_ts->value(), 255,
                      ui->checkBox_inversion->isChecked() ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);
        //cv::adaptiveThreshold(cvImageBlur, cvImageToPrint, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);   

        qInfo() << "\tConversion de l'image en QImage..." ;
        qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_Grayscale8);


    }

    if(typeDeTraitement == aucun)
    {
        qInfo() << "\tAucun traitement d'image à effectuer.";
        cv::cvtColor( *cvImage, cvImageToPrint, cv::COLOR_BGR2RGB);

        qInfo() << "\tConversion de l'image en QImage..." ;
        qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_RGB888);
    }
}








