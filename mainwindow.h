/*  File : mainwindow.h

    Description : fichier header pour une classe gérant l'interface graphique du programme

    Dépend de :     - librairie OpenCV
                    - Framework Qt
*/

/*************************************************************************************************\
|*******************************                                 *********************************|
|*******************************          PREPROCESSEUR          *********************************|
|*******************************                                 *********************************|
\*************************************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/****************************************
 *  Qt includes
 * *************************************/
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QPixmap>
#include <QGraphicsView>
#include <qstring.h>
#include <QDebug>
#include "camera_support.h"

#include "calibration_window.h"

/****************************************
 *  OpenCV includes
 * *************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "staubli_manager.h"
#include "im_traitement.h"
#include "px2mm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*************************************************************************************************\
|*******************************                                 *********************************|
|*******************************     DEFINITION DE LA CLASSE     *********************************|
|*******************************                                 *********************************|
\*************************************************************************************************/
/**
 * @brief The MainWindow class : permet la gestion de l'interface
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

/****************************************
 *  Méthodes publiques
 * *************************************/
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

/****************************************
 *  SLOT privées
 * *************************************/
private slots:
        /**
         * @brief onInitClick : Met à jour l'interface et initialise la caméra
         */
    void onInitClick();
        /**
         * @brief onCalibrateClick : lance la fenetre de calibrage
         */
    void onCalibrateClick();
        /**
         * @brief onSingleClick : Met à jour l'interface et effectue une capture avec la caméra
         */
    void onSingleClick();
        /**
         * @brief onImgPreClick : permet de réaliser le traitement préliminaire de l'image et de l'afficher
         */
    void onImgPreClick();
        /**
         * @brief onDiscoClick : Met à jour l'interface et déconnecte la caméra
         */
    void onDiscoClick();

        /**
         * @brief onResButtonChange0 : met à jour les attributs de résolution pour la prochaine initialisation
         */
    void onResButtonChange0();
        /**
         * @brief onResButtonChange1 : met à jour les attributs de résolution pour la prochaine initialisation
         */
    void onResButtonChange1();
        /**
         * @brief onResButtonChange2 : met à jour les attributs de résolution pour la prochaine initialisation
         */
    void onResButtonChange2();
        /**
         * @brief onResButtonChange3 : met à jour les attributs de résolution pour la prochaine initialisation
         */
    void onResButtonChange3();

        /**
         * @brief onSliderChange : permet de mettre à jour l'attribut cvThreshold
         */
    void onSliderChange();
        /**
         * @brief onSliderChange2 : permet de mettre à jour l'attribut cvRatio
         */
    void onSliderChange2();
    /**
     * @brief onCkBxContoursChange : met à jour la variable imTraitementType (flag)
     * @param state
     */
    void onCkBxContoursChange(int state);
    /**
     * @brief onCkBxContoursChange : met à jour la variable imTraitementType (flag)
     * @param state
     */
    void onCkBxSeuillageChange(int state);

    void onGeneTrajClick();

    void onOffsetChange(QVector3D offs, QVector3D currentStaubliPoint, QPoint dimIm);

    void onPx2mmClick();

    void onPush2Staubli();

/****************************************
 *  Attributs privés
 * *************************************/
private:
    /**
     * @brief setRadioBtnState : permet de désactiver ou d'activer tous les radioButtonRes d'un coup
     * @param state : bool indiquant l'état des bouttons
     */
    void setRadioBtnState(bool state);
    Ui::MainWindow *ui;
    QRadioButton *radioRes[4];

    im_traitement im_trait;

    std::vector<std::vector<cv::Point>> trajectories;
    std::vector<std::vector<QVector3D>> trajectoriesRobot;

    QVector3D offsets;
    QVector3D centralPoint;
    QPoint dimImage;

    calibration_window *w2;

    staubli_manager staubli;

    QImage qImage;
    QImage qImagePre;
    QImage qImageSkel;
    QImage qImageRes;

    camera_support camera;

    int cvThreshold;
    int cvRatio;

    cv::Mat *cvImage;
    cv::Mat cvImagePre;
    cv::Mat cvImageSkel;
    cv::Mat cvImageRes;
    cv::Mat cvImageToPrint;
    /**
     * @brief The typeDeTraitement enum : désigne un type de traitement d'image
     */
    enum enumTypeDeTraitement{aucun = 0, segmentation = 1, contours = 2, seuillage = 4};
    enumTypeDeTraitement typeDeTraitement = aucun;

    /**
     * @brief findCenter : trouve le point tq 95% des points blancs sont dedans avec un rayon minimum
     * @return les coordonnées trouvées
     */
    cv::Point findCenter(cv::Mat image, cv::Point *inf, cv::Point *sup);
    cv::Point centerPoint;
};
#endif // MAINWINDOW_H
