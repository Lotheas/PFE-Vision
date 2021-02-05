#ifndef CALIBRATION_WINDOW_H
#define CALIBRATION_WINDOW_H

/****************************************
 *  Qt includes
 * *************************************/
#include <QMainWindow>
#include <QInputDialog>
#include <QDebug>

/****************************************
 *  OpenCV includes
 * *************************************/

#include <opencv2/core/core.hpp>
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc.hpp>

#include "camera_support.h"
#include "staubli_manager.h"

namespace Ui {
class calibration_window;
}

class calibration_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit calibration_window(QWidget *parent = nullptr);
    calibration_window(QWidget *parent = nullptr, camera_support *cam = NULL, staubli_manager *stbl = NULL);
    ~calibration_window();

private:
    Ui::calibration_window *ui;
    /****************************************
     *  Atttributs
     * *************************************/
    ///
    /// \brief image : image ou l'on detectera le cercle au format OpenCV
    ///
    cv::Mat image;

    ///
    /// \brief camera : permet d'aqerir des images avec l'API basler
    ///
    camera_support camera;

    ///
    /// \brief staubli : permet de gérer les mouvements du staubli
    ///
    staubli_manager *staubli;

    ///
    /// \brief circleCoord : coordonnées du centre du cercle en pixels
    ///
    cv::Point circleCoord;

    ///
    /// \brief circleRadius : rayon du cercle en pixels
    ///
    ushort circleRadius;

    ///
    /// \brief circles : permet de recueillir les cercle de Hough
    ///
    std::vector<cv::Vec3f> circles;

    ///
    /// \brief qImage : image ou l'on detectera le cercle au format Qt
    ///
    QImage qImage;

    ///
    /// \brief focale : distance focale de l'objectif
    ///
    double focale;

    ///
    /// \brief dimX, dimY : dimensions de l'image en pixel
    ///
    uint dimX, dimY;

    ///
    /// \brief senDimX, senDimY : dimensions du capteurs en mm
    ///
    double senDimX, senDimY;

    ///
    /// \brief diamCercle : diamètre du cercle utilisé pour le calibrage en mm
    ///
    double diamCercle;

    ///
    /// \brief distSensor : distance entre le capteur et la feuille en mm
    ///
    double distH;

    ///
    /// \brief offsetX : les calibrages selon les 3 axes
    ///
    double offsetX, offsetY, offsetZ;


    QVector3D offsets;


    /****************************************
     *  Méthodes
     * *************************************/
    ///
    /// \brief Affichage d'une information dans la console
    ///
    void print(QString);
    ///
    /// \brief changeData : met à jour la valeur en fonction du retour de la textebox
    /// \param titre
    /// \param message
    /// \param value
    /// \param txtBox
    ///
    void changeData(QString titre, QString message, uint *value, QString varName, QString varUnit, int min, int max , QLineEdit *txtBox);
    ///
    /// \brief changeData : met à jour la valeur en fonction du retour de la textebox
    /// \param titre
    /// \param message
    /// \param value
    /// \param txtBox
    ///
    void changeData(QString titre, QString message, double *value, QString varName, QString varUnit, double min, double max, QLineEdit *txtBox);
    ///
    /// \brief printImage : affiche l'image passée en paramètre dans le label_image
    /// \param img
    ///
    void printImage(cv::Mat img);
    ///
    /// \brief drawCircle : créé un cercle sur l'image passée en paramètre
    /// \param img
    /// \return
    ///
    cv::Mat drawCircle(cv::Mat img);

private slots :
    /****************************************
     *  slots
     * *************************************/
    ///
    /// \brief on_draw : quand on clique sur le bouton permettant de dessiner le premier cercle
    ///
    void on_draw();
    ///
    /// \brief on_focale_change : permet de mettre à jour la distance focale avec une boite de dialogue
    ///
    void on_focaleChange();
    ///
    /// \brief on_dimXChange : change la dimnesion max en X  en pixels
    ///
    void on_dimXChange();
    ///
    /// \brief on_dimYChange : change la dimnesion max en Y en pixels
    ///
    void on_dimYChange();
    ///
    /// \brief on_senDimXChange : change la dimension du capteur en X en mm
    ///
    void on_senDimXChange();
    ///
    /// \brief on_senDimYChange : change la dimension du capteur en Y en mm
    ///
    void on_senDimYChange();
    ///
    /// \brief on_diamCircleChange : change le diametre du cercle de calibrage en mm
    ///
    void on_diamCircleChange();
    ///
    /// \brief on_distHChanged : change la distance feuille-capteur en mm
    ///
    void on_distHChange();

    ///
    /// \brief on_arrowClick : permet de déplacer le centre du cercle
    ///
    void on_arrowClick();

    ///
    /// \brief on_radiusChange : permet d'ajuste le rayon du cercle
    ///
    void on_radiusChange();

    ///
    /// \brief on_robotCoordChange : permet de modifier les coordonnées du robot
    ///
    void on_robotCoordChange();

    ///
    /// \brief on_takePicture : permet de prendre une photo avec la camera Basler
    /// \param imgRes
    ///
    void on_takePicture();

    ///
    /// \brief on_calibrate : effectue les calculs pour le calibrage en ofnction des données entrées
    ///
    void on_calibrate();

    ///
    /// \brief on_editOffX : permet de modifier les paramètre calculés
    ///
    void on_editOffX();

    ///
    /// \brief on_editOffY : permet de modifier les paramètre calculés
    ///
    void on_editOffY();

    ///
    /// \brief on_editOffZ : permet de modifier les paramètre calculés
    ///
    void on_editOffZ();

signals:
    void offsetsChanged(QVector3D);
};

#endif // CALIBRATION_WINDOW_H
