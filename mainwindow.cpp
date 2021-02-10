#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

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
    ui->lbl_imgInit->setBackgroundRole(QPalette::Base);
    ui->lbl_imgInit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->lbl_imgInit->setScaledContents(true);

    //Pour utiliser une boucle for pour toggle leur état
    radioRes[0] = ui->radioBtn_3088x2064;
    radioRes[1] = ui->radioBtn_2316x1548;
    radioRes[2] = ui->radioBtn_1544x1032;
    radioRes[3] = ui->radioBtn_772x516;

    //Button connections
    QObject::connect(ui->btn_init, SIGNAL(clicked()), this, SLOT(onInitClick()));
    QObject::connect(ui->btn_single, SIGNAL(clicked()), this, SLOT(onSingleClick()));
    QObject::connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(onDiscoClick()));
    QObject::connect(ui->btn_calibrage, SIGNAL(clicked()), this, SLOT(onCalibrateClick()));
    QObject::connect(ui->btn_imgPre, SIGNAL(clicked()), this, SLOT(onImgPreClick()));

    //Radio Button
    QObject::connect(ui->radioBtn_3088x2064, SIGNAL(clicked()), this, SLOT(onResButtonChange0()));
    QObject::connect(ui->radioBtn_2316x1548, SIGNAL(clicked()), this, SLOT(onResButtonChange1()));
    QObject::connect(ui->radioBtn_1544x1032, SIGNAL(clicked()), this, SLOT(onResButtonChange2()));
    QObject::connect(ui->radioBtn_772x516, SIGNAL(clicked()), this, SLOT(onResButtonChange3()));

    //Threshold traitement image
    cvThreshold = 0;
    cvRatio = 3;
    QObject::connect(ui->slider_ts, &QSlider::valueChanged, this, &MainWindow::onSliderChange);
    QObject::connect(ui->slider_ratio, &QSlider::valueChanged, this, &MainWindow::onSliderChange2);

    QObject::connect(ui->btn_traj, SIGNAL(clicked()), this, SLOT(onGeneTrajClick()));

    QObject::connect(ui->btn_px2mm, SIGNAL(clicked()), this, SLOT(onPx2mmClick()));

    QObject::connect(ui->btn_pushStaubli, SIGNAL(clicked()), this, SLOT(onPush2Staubli()));

    //Allocation dynamique de cvImage; devra être "delete"
    cvImage = new cv::Mat();

    //MAJ de l'interface en fonction de l'état de la connexion avec le robot
    if(staubli.online())
    {
        ui->lbl_staubli_txt->setText("Staubli online");
        ui->lbl_staubli_color->setStyleSheet("background-color: rgb(0, 255, 0);");
    }
    else
    {
        ui->lbl_staubli_txt->setText("Staubli offline");
        ui->lbl_staubli_color->setStyleSheet("background-color: rgb(255, 0, 0);");
    }
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
        ui->btn_calibrage->setEnabled(true);
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


    }
    else
    {
        QMessageBox msg;
        msg.setText("Merci de sélectionner une résolution!");
        msg.exec();
    }
}

void MainWindow::onCalibrateClick()
{

    ui->btn_calibrage->setEnabled(false);
    w2->setAttribute(Qt::WA_DeleteOnClose);
    w2->show();

    // This loop will wait for the window is destroyed
    QEventLoop loop;
    connect(w2, SIGNAL(destroyed()), & loop, SLOT(quit()));
    loop.exec();

    staubli.goHome();
    staubli.goToTarget();

    qInfo() << "Offsets from main : " << offsets.x() << " : " << offsets.y() << " : " << offsets.z();
}

void MainWindow::onSingleClick()
{

    qInfo() << "Function \"take_picture\" ";
    if(camera.take_picture(cvImage))
    {
        ui->btn_imgPre->setEnabled(true);

        qInfo() << "\tDimensions de l'image source : " << cvImage->rows << " x " << cvImage->cols;
        qInfo() << "\tChannels l'image source : " << cvImage->channels();

        cv::cvtColor(*cvImage, cvImageToPrint, cv::COLOR_BGR2RGB);

        qInfo() << "\tConversion de l'image en QImage..." ;
        qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_RGB888);

        QPixmap pixmap(qImage.width(), qImage.height());

        qInfo() << "===================================================";

        qInfo() << "Convertion de l'image en Pixmap";
        pixmap = QPixmap::fromImage(qImage);
        pixmap = pixmap.scaled(ui->lbl_imgInit->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

        qInfo() << "MAJ du label... ";
        ui->lbl_imgInit->setPixmap(pixmap);
        qInfo() << "Done. ";

//        qInfo() << "Label size : ";
//        qInfo() << "\t" << ui->lbl_imgInit->width();
//        qInfo() << "\t" << ui->lbl_imgInit->height();
    }
    else
    {
        QMessageBox msg;
        msg.setText("Grab not succeded!");
        msg.exec();
    }

}

void MainWindow::onImgPreClick()
{
    ui->btn_traj->setEnabled(true);

    qInfo() << "\tSeuillage de l'image...";

    cv::cvtColor( *cvImage, cvImagePre, cv::COLOR_BGR2GRAY);

    //cv::equalizeHist(cvImagePre, cvImagePre);

    cv::Mat mask = cv::Mat::zeros(cvImagePre.rows,cvImagePre.cols,CV_8U);

    float maxdist = std::sqrt(cvImagePre.rows*cvImagePre.rows+cvImagePre.cols*cvImagePre.cols)/2;

    cv::Point2f center(cvImagePre.cols*0.5,cvImagePre.rows*0.5);

    for (int j=0;j<cvImagePre.rows;++j)
    {
        for (int i=0;i<cvImagePre.cols;++i)
        {
             cv::Point2f p(i,j);
             cv::Point2f diff(p-center);
             float dist(std::sqrt(diff.dot(diff)));
             float factor(255*(0.8+0.4*dist/maxdist));
             mask.at<uchar>(j,i) = factor;
        }
    }

    //cv::imshow("hey", cvImagePre);

    //cv::imshow("coucou", cvImagePre);
    //cv::blur( cvImagePre, cvImagePre, cv::Size(5,5));
    cv::threshold(cvImagePre, cvImagePre, ui->slider_ts->value(), 255,
                  ui->checkBox_inversion->isChecked() ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY );
    //cv::adaptiveThreshold(cvImagePre, cvImagePre, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2); //ui->slider_ts->value(), ui->slider_ratio->value());

    qInfo() << "\tConversion de l'image en QImage..." ;
    qImagePre = QImage((unsigned char*)cvImagePre.data, cvImagePre.cols, cvImagePre.rows, QImage::Format_Grayscale8);

    QPixmap pixmap;
    pixmap = QPixmap::fromImage(qImagePre);
    pixmap = pixmap.scaled(ui->lbl_imgPre->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

    qInfo() << "MAJ du label... ";
    ui->lbl_imgPre->setPixmap(pixmap);
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

void MainWindow::onGeneTrajClick()
{
    if(!cvImagePre.empty())
    {
        ui->btn_pushStaubli->setEnabled(true);
        trajectories = im_trait.trajectory_generation(cvImagePre);

        cvImageRes = cv::Mat(cvImagePre.size(), CV_8UC3, cv::Scalar(0));

        //initialisation pour l'utilisation de rand()
        srand(time(NULL));

        //On créé l'image permettant de visualiser les points retenus pour la trajectoire
        for (uint i = 0; i < trajectories.size(); i++)
        {
            cv::Vec3b couleur(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);

            for (uint j = 0; j < trajectories[i].size(); j++)
            {

                cvImageRes.at<cv::Vec3b>(trajectories[i][j]) = couleur;

                cv::circle(cvImageRes, trajectories[i][j], 5, couleur, 5);
            }
        }

        //On affiche l'image
        cv::cvtColor(cvImageRes, cvImageRes, cv::COLOR_BGR2RGB);

        qImageRes = QImage((unsigned char*)cvImageRes.data, cvImageRes.cols, cvImageRes.rows, QImage::Format_RGB888);

        QPixmap pixmap;
        pixmap = QPixmap::fromImage(qImageRes);
        pixmap = pixmap.scaled(ui->lbl_imgRes->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

        qInfo() << "MAJ du label... ";
        ui->lbl_imgRes->setPixmap(pixmap);

        //On affiche les points dans la table prévue à cet effet
        //On clear les lignes du tableau
        uint nbRows = ui->table_points->rowCount();

        for(uint i = 0; i < nbRows; i++)
        {
            ui->table_points->removeRow(0);
        }

        //On le rempli avec les nouvelles données
        for (uint i = 0; i < trajectories.size(); i++)
        {

            for (uint j = 0; j < trajectories[i].size(); j++)
            {
                ui->table_points->insertRow(ui->table_points->rowCount());

                ui->table_points->setItem(  ui->table_points->rowCount()-1,
                                            0,
                                            new QTableWidgetItem(QString::number(i)));
                ui->table_points->setItem(  ui->table_points->rowCount()-1,
                                            1,
                                            new QTableWidgetItem("(" + QString::number(trajectories[i][j].x) +
                                                                 " ; " + QString::number(trajectories[i][j].y) + ")"));
            }
        }
    }
    else
    {
        QMessageBox msg;
        msg.setText("Merci d'effectuer le traitement préliminaire.");
        msg.exec();
    }
}

void MainWindow::onOffsetChange(QVector3D offs, QVector3D currentStaubliPoint, QPoint dimIm)
{
    offsets = offs;
    dimImage = dimIm;

    //L'axe X du robot correspond à l'axe y de l'image et inversement (normalement dans le meme sens)
    centralPoint.setX(currentStaubliPoint.x() - offs.y() - 4);

    centralPoint.setY(currentStaubliPoint.y() - offs.x() + 2);

    centralPoint.setZ(currentStaubliPoint.z() - offs.z() + 203);
}

void MainWindow::onPx2mmClick()
{
    QPoint reso(cvImage->cols, cvImage->rows);
    trajectoriesRobot = px2mm::convertToMm(trajectories, centralPoint, reso, dimImage);

    uint k = 0;

    for (uint i = 0; i < trajectoriesRobot.size(); i++)
    {

        for (uint j = 0; j < trajectoriesRobot[i].size(); j++)
        {
            ui->table_points->setItem(  k,
                                        2,
                                        new QTableWidgetItem("(" + QString::number(trajectoriesRobot[i][j].x()) +
                                                             " ; " + QString::number(trajectoriesRobot[i][j].y()) +
                                                             " ; " + QString::number(trajectoriesRobot[i][j].z()) +
                                                             ")"));
            k++;
        }
    }

}

void MainWindow::onPush2Staubli()
{
    if(staubli.online())
    {
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(ui->table_points->rowCount());
        //Sending data
        for (uint i = 0; i < trajectoriesRobot.size(); i++)
        {

            for (uint j = 0; j < trajectoriesRobot[i].size(); j++)
            {
                if(j == 0) //Premeire data on fait l'appro
                {
                    staubli.setTargetPoint( trajectoriesRobot[i][j].x(), trajectoriesRobot[i][j].y(), trajectoriesRobot[i][j].z() + 10);
                    staubli.goToTarget();
                    QThread::msleep(100);
                }

                staubli.setTargetPoint( trajectoriesRobot[i][j]);
                staubli.goToTarget();
                QThread::msleep(100);
                ui->progressBar->setValue(ui->progressBar->value() + 1);

                if(j == trajectoriesRobot[i].size() - 1)//Derniere data on fait l'appro
                {

                    staubli.setTargetPoint( trajectoriesRobot[i][j].x(), trajectoriesRobot[i][j].y(), trajectoriesRobot[i][j].z() + 10);
                    staubli.goToTarget();
                    QThread::msleep(100);
                }

            }
        }
        staubli.goHome();
        staubli.goToTarget();

    }
    else
    {
        QMessageBox msg;
        msg.setText("Staubli offline, action impossible.");
        msg.exec();
    }
}










