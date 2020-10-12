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

/****************************************
 *  OpenCV includes
 * *************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
         * @brief onSingleClick : Met à jour l'interface et effectue une capture avec la caméra
         */
    void onSingleClick();
        /**
         * @brief onVideoClick : Met à jour l'interface
         */
    void onVideoClick();
        /**
         * @brief onStopClick : Met à jour l'interface
         */
    void onStopClick();
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

    QImage qImage;

    camera_support camera;

    int cvThreshold;
    int cvRatio;

    cv::Mat *cvImage;
};
#endif // MAINWINDOW_H
