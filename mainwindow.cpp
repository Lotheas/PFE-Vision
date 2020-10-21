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

    //CheckBox Traitement image
    QObject::connect(ui->checkBox_contours, SIGNAL(stateChanged(int)), this, SLOT(onCkBxContoursChange(int)));
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
    //Déconnect la caméra
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

/***************************************************************
 *                      TRAITEMENT DE L'IMAGE
 * ************************************************************/
void MainWindow::imageTraitement()
{
    qInfo() << "Traitement de l'image : ";

    //Déclaration de l'image qui sera ensuite affichée dans l'UI
    cvImageToPrint.create(cvImage->rows, cvImage->cols, CV_8UC3);

    qInfo() << "\tDimensions de l'image destination : " << cvImageToPrint.rows << " x " << cvImageToPrint.cols;
    qInfo() << "\tChannels de l'image destination : " << cvImageToPrint.channels();

    if(typeDeTraitement & contours)
    {
        qInfo() << "\tDétection de contours...";

        cv::Mat cvImageGray, cvImageBlur;
        qInfo() << "\tTraitement de l'image...";

        if(cvImage->type() != CV_8U)
            cv::cvtColor( *cvImage, cvImageGray, cv::COLOR_BGRA2GRAY);
        cv::blur( cvImageGray, cvImageBlur, cv::Size(5,5));
        cv::Canny(cvImageBlur, cvImageToPrint, cvThreshold, cvRatio*cvThreshold, 3);

        qInfo() << "\tConversion de l'image en QImage..." ;
        qImage = QImage((unsigned char*)cvImageToPrint.data, cvImageToPrint.cols, cvImageToPrint.rows, QImage::Format_Grayscale8);
    }

    if(typeDeTraitement & seuillage)
    {
        qInfo() << "\tDétection de contours...";

        cv::Mat cvImageGray, cvImageBlur;
        qInfo() << "\tTraitement de l'image...";

        if(cvImage->type() != CV_8U)
            cv::cvtColor( *cvImage, cvImageGray, cv::COLOR_BGRA2GRAY);
        cv::blur( cvImageGray, cvImageBlur, cv::Size(3,3));
        cv::threshold(cvImageBlur,cvImageToPrint, 127, 255,
                      ui->checkBox_inversion->isChecked() ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);
        //cv::adaptiveThreshold(cvImageBlur, cvImageToPrint, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);

//        uint8_t kdata[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
//        cv::Mat kernel(3,3, CV_8U, kdata);
//        //cv::morphologyEx(cvImageToPrint, cvImageToPrint, cv::MORPH_OPEN, kernel);

        if(ui->checkBox_coord->checkState() == Qt::Checked)
        {

            cv::Point infGauche;
            cv::Point supDroit;

            centerPoint = findCenter(cvImageToPrint, &infGauche, &supDroit);

            cv::rectangle(cvImageToPrint, infGauche, supDroit, cv::Scalar(255), 3);

            qInfo() << "\t Point détecté aux coordonnées :\t" << centerPoint.x << "  x  " << centerPoint.y;

            int edgeDistance = 5;

            cv::line(cvImageToPrint, cv::Point(centerPoint.x,edgeDistance), cv::Point(centerPoint.x, cvImageToPrint.rows - edgeDistance), cv::Scalar(255), 3);
            cv::line(cvImageToPrint, cv::Point(edgeDistance,centerPoint.y), cv::Point( cvImageToPrint.cols - edgeDistance, centerPoint.y), cv::Scalar(255), 3);

        }
        else if(ui->checkBox_coord->checkState() == Qt::PartiallyChecked)
        {
            std::vector<cv::Vec3f> circles;
            HoughCircles(cvImageToPrint, circles, cv::HOUGH_GRADIENT, 1,
                         cvImageToPrint.rows/16,  // change this value to detect circles with different distances to each other
                         100, 30, 1, 500); // change the last two parameters
            // (min_radius & max_radius) to detect larger circles

            qInfo() << "\t\t" << circles.size() << "\t cercles détectés";

            for( size_t i = 0; i < circles.size(); i++ )
            {
                cv::Vec3i c = circles[i];
                cv::Point center = cv::Point(c[0], c[1]);
                // circle center
                cv::circle( cvImageToPrint, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
                // circle outline
                int radius = c[2];
                cv::circle( cvImageToPrint, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);

                cv::line(cvImageToPrint, cv::Point(center.x,0), cv::Point(center.x, cvImageToPrint.rows), cv::Scalar(255));
                cv::line(cvImageToPrint, cv::Point(0,center.y), cv::Point( cvImageToPrint.cols, center.y), cv::Scalar(255));
            }
        }

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

/***********************
 *      findCenter
 * ********************/

cv::Point MainWindow::findCenter(cv::Mat image, cv::Point *inf, cv::Point *sup)
{
    qInfo() << "Début de la détection de la zone ";
    cv::Point result(0,0);

    //Récupération de l'image dans un tableau de données
    qInfo() << "\tExtraction des données de l'image";
    int dimx, dimy;
    dimx = image.cols;
    dimy = image.rows;
    int nbrPixels = 0;

    qInfo() << "\tCompte du nombre de pixels valides";
    //Obtient le nombre de pixels blancs
    for(int y = 0; y < dimy; y++)
    {
        for(int x = 0; x < dimx; x++)
        {
            if(image.at<uchar>(y, x))
                nbrPixels++;
        }
    }
    qInfo() << "\tNombre de pixels total  :\t" << dimx*dimy;
    qInfo() << "\tNombre de pixels blancs :\t" << nbrPixels;

    //On retiendra 95% des points blancs
    nbrPixels = (int)((double)nbrPixels * ui->slider_ts->value()/100);
    qInfo()  << "\tNombre de pixels blancs 95% :\t" << nbrPixels;

    //On fonctionne en dichotomie sur x et y
    int actualNbr = 0; //Nbr de pixels dans une zone
    int nextRegion = 0; //Indice de la procahine région (0 si aucune)

    bool regionTrouvee = false; //Flag si plus aucune région ne contient 95% des pixels blancs

    //Bornes pour la dichotomie
    int xSup = dimx, ySup = dimy;
    int xInf = 0, yInf = 0;

    qInfo() << "Début de recherceh de la region";
    int compteur = 1;
    //Tant que la region n'est pas trouvée
    while(!regionTrouvee)
    {
        qInfo() << "\tItération n° " << compteur;
        actualNbr = 0;
        nextRegion = 0;

        /***********************
         *      Region 1 Bas gauche
         * ********************/

        for(int x = xInf; x < (xSup + xInf) / 2; x++) //Compte pour chaque région le nombre de pixels blancs
        {
            for(int y = yInf; y < (ySup + yInf) / 2; y++)
            {
                if(image.at<uchar>(y, x))
                    actualNbr++;
            }
        }

        if(actualNbr >= nbrPixels) //Test du nombre de pixels
            nextRegion = 1;

        qInfo() << "\t\tRegion 1 : \t" << actualNbr << " / " << nbrPixels;

        if(nextRegion == 0)
        {
            /***********************
             *      Region 2 Bas droit
             * ********************/
            actualNbr = 0;

            for(int x = (xSup + xInf) / 2; x < xSup; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = yInf; y < (ySup + yInf) / 2; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 2;

            qInfo() << "\t\tRegion 2 : \t" << actualNbr << " / " << nbrPixels;
        }


        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 3 haut gauche
             * ********************/
            for(int x = xInf; x < (xSup + xInf) / 2; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = (ySup + yInf) / 2; y < ySup; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 3;

            qInfo() << "\t\tRegion 3 : \t" << actualNbr << " / " << nbrPixels;
        }

        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 4 haut droit
             * ********************/
            for(int x = (xSup + xInf) / 2; x < xSup; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = (ySup + yInf) / 2; y < ySup; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 4;

            qInfo() << "\t\tRegion 4 : \t" << actualNbr << " / " << nbrPixels;
        }

        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 5 bas
             * ********************/
            for(int x = xInf; x < xSup; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = yInf; y < (ySup + yInf) / 2; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 5;

            qInfo() << "\t\tRegion 5 : \t" << actualNbr << " / " << nbrPixels;
        }

        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 6 haut
             * ********************/
            for(int x = xInf; x < xSup; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = (ySup + yInf) / 2; y < ySup; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 6;

            qInfo() << "\t\tRegion 6 : \t" << actualNbr << " / " << nbrPixels;
        }

        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 7 gauche
             * ********************/
            for(int x = xInf; x < (xSup + xInf) / 2; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = yInf; y < ySup; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 7;

            qInfo() << "\t\tRegion 7 : \t" << actualNbr << " / " << nbrPixels;
        }

        if(nextRegion == 0)
        {
            actualNbr = 0;
            /***********************
             *      Region 8 droit
             * ********************/
            for(int x = (xSup + xInf) / 2; x < xSup; x++) //Compte pour chaque région le nombre de pixels blancs
            {
                for(int y = yInf; y < ySup; y++)
                {
                    if(image.at<uchar>(y, x))
                        actualNbr++;
                }
            }

            if(actualNbr >= nbrPixels) //Test du nombre de pixels
                nextRegion = 8;

            qInfo() << "\t\tRegion 8 : \t" << actualNbr << " / " << nbrPixels;
        }

        /***********************************************
         *      Détermination de la nouvelle région
         * *********************************************/
        if(nextRegion == 0)
            regionTrouvee = true;
        else
        {
            qInfo() << "\t\tNextRegion : \t" << nextRegion;
            switch(nextRegion){
            case 1 :
                xSup = (xSup + xInf) / 2;
                ySup = (ySup + yInf) / 2;
                break;
            case 2 :
                xInf = (xSup + xInf) / 2;
                ySup = (ySup + yInf) / 2;
                break;
            case 3 :
                xSup = (xSup + xInf) / 2;
                yInf = (ySup + yInf) / 2;
                break;
            case 4 :
                xInf = (xSup + xInf) / 2;
                yInf = (ySup + yInf) / 2;
                break;
            case 5 :
                ySup = (ySup + yInf) / 2;
                break;
            case 6 :
                yInf = (ySup + yInf) / 2;
                break;
            case 7 :
                xSup = (xSup + xInf) / 2;
                break;
            case 8 :
                xInf = (xSup + xInf) / 2;
                break;
            default :
                qInfo() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
                qInfo() << "!                                                                                                                       !";
                qInfo() << "!   Dommage! Le mec qui a codé ça est une grosse tanche! : \"nextRegion\" est foireux dans la fonction \"findCenter\"   !";
                qInfo() << "!                                                                                                                       !";
                qInfo() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            }
        }

        qInfo() << "\tCoordonnées retenues : ";
        qInfo() << "\t\tX\t Inf :" << xInf;
        qInfo() << "\t\t\t\t Sup :" << xSup;
        qInfo() << "\t\tY\t Inf :" << yInf;
        qInfo() << "\t\t\t\t Sup :" << ySup;
        compteur++;

    }

    qInfo() << "\t+++ Recherche terminée +++";
    result = cv::Point((xSup + xInf)/2, (ySup + yInf)/2 );

    inf->x = xInf;
    inf->y = yInf;
    sup->x = xSup;
    sup->y = ySup;

    return result;
}






