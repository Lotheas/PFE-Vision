#include "calibration_window.h"
#include "ui_calibration_window.h"

using namespace std;
using namespace cv;

/**********************************************************************************************
                             _          _                   _
                          /\| |/\      | |                 | |
                          \ ` ' /   ___| |_ _ __ _   _  ___| |_ ___  _ __
                         |_     _  / __| __| '__| | | |/ __| __/ _ \| '__|
                          / , . \  \__ \ |_| |  | |_| | (__| || (_) | |
                          \/|_|\/  |___/\__|_|   \__,_|\___|\__\___/|_|
 *
 * ********************************************************************************************/
calibration_window::calibration_window(QWidget *parent, camera_support *cam, staubli_manager *stbl) :
    QMainWindow(parent),
    ui(new Ui::calibration_window)
{
    ui->setupUi(this);

    camera = *cam;
    staubli = stbl;

    if(staubli->online())
    {
        ui->lbl_staubliOnlineTxt->setText("Online");
        ui->lbl_staubliOnlineColor->setStyleSheet("background-color: rgb(0, 255, 0);");
    }
    else
    {
        ui->lbl_staubliOnlineTxt->setText("Offline");
        ui->lbl_staubliOnlineColor->setStyleSheet("background-color: rgb(255, 0, 0);");
    }

    //Valeurs par défaut
    focale = 12.0;
    dimX = camera.getResolution()[0];
    dimY = camera.getResolution()[1];
    senDimX = 7.41597183;
    senDimY = 4.95678946;
    diamCercle = 120;
    distH = 600;

    offsetX = 0;
    offsetY = 0;
    offsetZ = 0;

    QVector3D pt = staubli->getCurrentPoint();
    ui->txtBox_robotX->setText(QString::number(pt.x()));
    ui->txtBox_robotY->setText(QString::number(pt.y()));
    ui->txtBox_robotZ->setText(QString::number(pt.z()));

    //Affichage des valeurs par défaut
    ui->txt_camSet_focale->setText(QString::number(focale));

    ui->txt_imgDim_x->setText(QString::number(dimX));
    ui->txt_imgDim_y->setText(QString::number(dimY));

    ui->txt_senDim_x->setText(QString::number(senDimX));
    ui->txt_senDim_y->setText(QString::number(senDimY));

    ui->txt_div_cercle->setText(QString::number(diamCercle));
    ui->txt_div_h->setText(QString::number(distH));

    //Connects
    QObject::connect(ui->btn_camSet_focale, SIGNAL(clicked()), this, SLOT(on_focaleChange()));

    QObject::connect(ui->btn_imgDim_x, SIGNAL(clicked()), this, SLOT(on_dimXChange()));
    QObject::connect(ui->btn_imgDim_y, SIGNAL(clicked()), this, SLOT(on_dimYChange()));

    QObject::connect(ui->btn_senDim_x, SIGNAL(clicked()), this, SLOT(on_senDimXChange()));
    QObject::connect(ui->btn_senDim_y, SIGNAL(clicked()), this, SLOT(on_senDimYChange()));

    QObject::connect(ui->btn_div_cercle, SIGNAL(clicked()), this, SLOT(on_diamCircleChange()));
    QObject::connect(ui->btn_div_h, SIGNAL(clicked()), this, SLOT(on_distHChange()));

    QObject::connect(ui->btn_initialCircle, SIGNAL(clicked()), this, SLOT(on_draw()));

    QObject::connect(ui->btn_adjustLeft, SIGNAL(clicked()), this, SLOT(on_arrowClick()));
    QObject::connect(ui->btn_adjustRight, SIGNAL(clicked()), this, SLOT(on_arrowClick()));
    QObject::connect(ui->btn_adjustUp, SIGNAL(clicked()), this, SLOT(on_arrowClick()));
    QObject::connect(ui->btn_adjustDown, SIGNAL(clicked()), this, SLOT(on_arrowClick()));

    QObject::connect(ui->btn_augDiam, SIGNAL(clicked()), this, SLOT(on_radiusChange()));
    QObject::connect(ui->btn_dimDiam, SIGNAL(clicked()), this, SLOT(on_radiusChange()));

    QObject::connect(ui->btn_robotXUp, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));
    QObject::connect(ui->btn_robotXDown, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));
    QObject::connect(ui->btn_robotYUp, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));
    QObject::connect(ui->btn_robotYDown, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));
    QObject::connect(ui->btn_robotZUp, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));
    QObject::connect(ui->btn_robotZDown, SIGNAL(clicked()), this, SLOT(on_robotCoordChange()));

    QObject::connect(ui->btn_photo, SIGNAL(clicked()), this, SLOT(on_takePicture()));

    QObject::connect(ui->btn_calibrate, SIGNAL(clicked()), this, SLOT(on_calibrate()));


    QObject::connect(ui->btn_offX, SIGNAL(clicked()), this, SLOT(on_editOffX()));


}

calibration_window::~calibration_window()
{
    delete ui;
}

/**********************************************************************************************
                                              _       _
                                             | |     | |
                                          ___| | ___ | |_ ___
                                         / __| |/ _ \| __/ __|
                                         \__ \ | (_) | |_\__ \
                                         |___/_|\___/ \__|___/
 *
 * ********************************************************************************************/
void calibration_window::on_draw()
{
    if(image.empty())
    {
        QMessageBox msg;
        msg.setWindowTitle("Erreur");
        msg.setText("Merci de prendre une première image");
        msg.exec();
    }
    else
    {
        Mat gray;

        cvtColor(image, gray, COLOR_BGR2GRAY);

        medianBlur(gray, gray, 5);

        HoughCircles(gray,
                     circles,
                     HOUGH_GRADIENT,
                     1,
                     gray.rows/16,
                     100,
                     25,
                     20,
                     200);

        ui->lbl_nbCircle->setText(QString::number(circles.size()) + " cercle trouvé.");

        if(circles.size() > 0)
        {
            circleCoord.x = circles[0][0];
            circleCoord.y = circles[0][1];
            circleRadius = circles[0][2];
        }
        else
        {
            circleCoord.x = dimX / 2;
            circleCoord.y = dimY / 2;
            circleRadius = min(dimX, dimY) / 8;
        }

        ui->txtBox_XCircle->setText(QString::number(circleCoord.x));
        ui->txtBox_YCircle->setText(QString::number(circleCoord.y));
        ui->txtBox_RCircle->setText(QString::number(circleRadius));

        printImage(drawCircle(image));
    }

}

void calibration_window::on_focaleChange()
{
    changeData("Distance focale",
               "Entrez la distance focale en mm :",
               &focale,
               "Focale",
               "mm",
               -100,
               100,
               ui->txt_camSet_focale);
}

void calibration_window::on_dimXChange()
{
    changeData("Dimension X max",
               "Entrez la dimension en X en pixels :",
               &dimX,
               "Dim X",
               "pixels",
               0,
               5000,
               ui->txt_imgDim_x);
}

void calibration_window::on_dimYChange()
{
    changeData("Dimension Y max",
               "Entrez la dimension en Y en pixels :",
               &dimY,
               "Dim Y",
               "pixels",
               0,
               5000,
               ui->txt_imgDim_y);
}

void calibration_window::on_senDimXChange()
{
    changeData("Sensor dimension",
               "Entrez dimension du capteur en X en mm :",
               &senDimX,
               "Sen Dim X",
               "mm",
               0,
               20,
               ui->txt_senDim_x);
}

void calibration_window::on_senDimYChange()
{
    changeData("Sensor dimension",
               "Entrez dimension du capteur en Y en mm :",
               &senDimY,
               "Sen Dim Y",
               "mm",
               0,
               20,
               ui->txt_senDim_y);
}

void calibration_window::on_diamCircleChange()
{
    changeData("Circle dimension",
               "Entrez dimension du cercle en mm :",
               &diamCercle,
               "Diam Circle",
               "mm",
               0,
               500,
               ui->txt_div_cercle);
}

void calibration_window::on_distHChange()
{
    changeData("Distance feuille-capteur",
               "Entrez la distance entre la feuille et le capteur en mm :",
               &distH,
               "Dist H",
               "mm",
               0,
               1000,
               ui->txt_div_h);
}

void calibration_window::on_arrowClick()
{
    if(image.empty())
    {
        QMessageBox msg;
        msg.setWindowTitle("Erreur");
        msg.setText("Merci de prendre une première image");
        msg.exec();
    }
    else
    {
        int pas = ui->txtBox_pas->text().toInt();

        if(QObject::sender()->objectName() == "btn_adjustUp")
        {
            circleCoord.y -= pas ;
            ui->txtBox_YCircle->setText(QString::number(circleCoord.y));
        }
        else if(QObject::sender()->objectName() == "btn_adjustDown")
        {
            circleCoord.y += pas;
            ui->txtBox_YCircle->setText(QString::number(circleCoord.y));
        }
        else if(QObject::sender()->objectName() == "btn_adjustRight")
        {
            circleCoord.x += pas;
            ui->txtBox_XCircle->setText(QString::number(circleCoord.x));
        }
        else if(QObject::sender()->objectName() == "btn_adjustLeft")
        {
            circleCoord.x -= pas;
            ui->txtBox_XCircle->setText(QString::number(circleCoord.x));
        }

        printImage(drawCircle(image));
    }
}

void calibration_window::on_radiusChange()
{
    if(image.empty())
    {
        QMessageBox msg;
        msg.setWindowTitle("Erreur");
        msg.setText("Merci de prendre une première image");
        msg.exec();
    }
    else
        {
        int pas = ui->txtBox_pas->text().toInt();

        if(QObject::sender()->objectName() == "btn_augDiam")
        {
            circleRadius += pas;
        }
        else
        {
            circleRadius -= pas;
        }

        ui->txtBox_RCircle->setText(QString::number(circleRadius));

        printImage(drawCircle(image));
    }
}

void calibration_window::on_robotCoordChange()
{
    int pas = ui->txtBox_pasRobot->text().toInt();

    if(QObject::sender()->objectName() == "btn_robotXUp") // X++
    {
        staubli->setRelativeTarget(pas, 0, 0);
        staubli->goToTarget();
    }
    else if(QObject::sender()->objectName() == "btn_robotXDown") // X--
    {
        staubli->setRelativeTarget(-pas, 0, 0);
        staubli->goToTarget();
    }
    else if(QObject::sender()->objectName() == "btn_robotYUp") // Y++
    {
        staubli->setRelativeTarget(0, pas, 0);
        staubli->goToTarget();
    }
    else if(QObject::sender()->objectName() == "btn_robotYDown") // Y--
    {
        staubli->setRelativeTarget(0, -pas, 0);
        staubli->goToTarget();
    }
    else if(QObject::sender()->objectName() == "btn_robotZUp") // Z++
    {
        staubli->setRelativeTarget(0, 0, pas);
        staubli->goToTarget();
    }
    else if(QObject::sender()->objectName() == "btn_robotZDown") // Z--
    {
        staubli->setRelativeTarget(0, 0, -pas);
        staubli->goToTarget();
    }

    QVector3D pt = staubli->getCurrentPoint();
    ui->txtBox_robotX->setText(QString::number(pt.x()));
    ui->txtBox_robotY->setText(QString::number(pt.y()));
    ui->txtBox_robotZ->setText(QString::number(pt.z()));

    on_takePicture();
}

void calibration_window::on_takePicture()
{
    if(camera.take_picture(&image))
    {
        printImage(image);
    }
    else
    {
        QMessageBox msg;
        msg.setText("Grab not succeded!");
        msg.exec();
    }
}

void calibration_window::on_calibrate()
{
    double val1 = distH - diamCercle * dimX * focale / (2 * circleRadius) / senDimX - focale;
    double val2 = distH - diamCercle * dimY * focale / (2 * circleRadius) / senDimY - focale;

    //print(" val 1 : " + QString::number(val1));
    //print(" val 2 : " + QString::number(val2));

    offsetZ = (val1 + val2) / 2;
    print("Offset Z : " + QString::number(offsetZ));

    double hc = distH - focale - offsetZ;

    double opx = circleCoord.x - (double)dimX / 2;
    double opy = circleCoord.y - (double)dimY / 2;
    //print("OP X : " + QString::number(opx));
    //print("OP Y : " + QString::number(opy));

    offsetX = senDimX / 2 * hc * opx / focale / (dimX / 2);
    print("Offset X : " + QString::number(offsetX));

    offsetY = senDimY / 2 * hc * opy / focale / (dimY / 2);

    print("Offset Y : " + QString::number(offsetY));

    ui->txt_offX->setText(QString::number(offsetX));
    ui->txt_offY->setText(QString::number(offsetY));
    ui->txt_offZ->setText(QString::number(offsetZ));
}

void calibration_window::on_editOffX()
{
    changeData("Correction Offset X",
               "Nouvelle valeur de Offset X en mm : ",
               &offsetX,
               "Offset X",
               "mm",
               -300,
               300,
               ui->txt_offX);

}

void calibration_window::on_editOffY()
{
    changeData("Correction Offset Y",
               "Nouvelle valeur de Offset Y en mm : ",
               &offsetY,
               "Offset Y",
               "mm",
               -300,
               300,
               ui->txt_offY);

}

void calibration_window::on_editOffZ()
{
    changeData("Correction Offset Z",
               "Nouvelle valeur de Offset Z en mm : ",
               &offsetZ,
               "Offset Z",
               "mm",
               -300,
               300,
               ui->txt_offX);

}
/**********************************************************************************************
                                          _ _
                                         | (_)
                                       __| |___   _____ _ __ ___
                                      / _` | \ \ / / _ \ '__/ __|
                                     | (_| | |\ V /  __/ |  \__ \
                                      \__,_|_| \_/ \___|_|  |___/

 * ********************************************************************************************/
void calibration_window::print(QString str)
{
    qInfo() << "\tCalibrage : " << str;
}

void calibration_window::changeData(QString titre, QString message,
                                    uint *value, QString varName, QString varUnit,
                                    int min, int max , QLineEdit *txtBox)
{
    //Var pour stocker le résultat de la QDialog
    uint res = 0.0;
    bool ok = false;

    res = QInputDialog::getInt(this, titre, message, 0, min, max, 1, &ok, Qt::WindowSystemMenuHint);

    //Si ok on cahnge la valeur en interne, sinon, on ne fait rien
    if(ok)
    {
        *value = res;
        txtBox->setText(QString::number(*value));
        print(varName + " changed to " + QString::number(*value) + " " + varUnit);
    }
    else
    {
        print(varName + " unchanged (" + QString::number(*value) + " " + varUnit + ")");
    }
}

void calibration_window::changeData(QString titre, QString message,
                                    double *value, QString varName, QString varUnit,
                                    double min, double max, QLineEdit *txtBox)
{
    //Var pour stocker le résultat de la QDialog
    double res = 0.0;
    bool ok = false;

    res = QInputDialog::getDouble(this, titre, message, 0, min, max, 5, &ok, Qt::WindowSystemMenuHint);

    //Si ok on cahnge la valeur en interne, sinon, on ne fait rien
    if(ok)
    {
        *value = res;
        txtBox->setText(QString::number(*value));
        print(varName + " changed to " + QString::number(*value) + " " + varUnit);
    }
    else
    {
        print(varName + " unchanged (" + QString::number(*value) + " " + varUnit + ")");
    }
}

void calibration_window::printImage(cv::Mat img)
{
    Mat imgTmp = img.clone();
    cvtColor(imgTmp, imgTmp, COLOR_BGR2RGB);
    qImage = QImage((uchar*) imgTmp.data, imgTmp.cols, imgTmp.rows, imgTmp.step, QImage::Format_RGB888);
    ui->lbl_image->setPixmap(QPixmap::fromImage(qImage));
    ui->lbl_image->setScaledContents(true);
}

cv::Mat calibration_window::drawCircle(cv::Mat img)
{
    Mat res(img.clone());

    circle(res, circleCoord, circleRadius, cv::Scalar(255,255,0), 2);
    line(res, Point(circleCoord.x,0), Point(circleCoord.x, res.rows - 1), Scalar(0,255,255), 1);
    line(res, Point(0, circleCoord.y), Point(res.cols - 1, circleCoord.y), Scalar(0,255,255), 1);

    return res.clone();
}
