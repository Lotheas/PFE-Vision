/*  File : camera_support.h

    Description : fichier header pour une classe permettant un interface simple avec la caméra Basler acA3088 - 57uc

    Dépend de :     - SDK Pylon 6
                    - librairie OpenCV
                    - Framework Qt
*/
/*************************************************************************************************\
|*******************************                                 *********************************|
|*******************************          PREPROCESSEUR          *********************************|
|*******************************                                 *********************************|
\*************************************************************************************************/
#ifndef CAMERA_SUPPORT_
#define CAMERA_SUPPORT_H

/****************************************
 *  Qt includes
 * *************************************/
#include <QMessageBox>
#include <QDebug>

/****************************************
 *  Pylon includes
 * *************************************/
#include "pylon/PylonIncludes.h"

/****************************************
 *  OpenCV includes
 * *************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

/*************************************************************************************************\
|*******************************                                 *********************************|
|*******************************     DEFINITION DE LA CLASSE     *********************************|
|*******************************                                 *********************************|
\*************************************************************************************************/

/**
 * Classe permettant une prise en charge simple de la caméra Basler
 *
 */
class camera_support
{
/****************************************
 *  Méthodes publiques
 * *************************************/
public:
        ///Constructeur de la classe "camera_support"
    camera_support();

        ///Destructeur de la classe "camera_support"
    ~camera_support();

        ///Permet d'inialiser la caméra (ou la première trouvée) avec les paramètres par défaut.
    void initilialize();

        /**
         * @brief take_picture : Permet de prendre une image avec la caméra Basler et la retourne au format BGR888.
         * @param result : image capturée par la caméra au format BGR888
         * @return bool : si la capture de la caméra s'est bien déroulée
         */
    bool take_picture(cv::Mat *result);

        /**
         * @brief change_resolution : permet la mise à jour des paramètres de résolution de la caméra. La fonction "initialize()" doit être rappelée pour que les changements soient effectifs.
         * @param width
         * @param height
         */
    void change_resolution(uint width, uint height);

        /**
         * @brief unInitialize : déconnecte la caméra. Peut être appelée avant de lancer à nouveau "Initialize()"
         */
    void unInitialize();


/****************************************
 *  Attributs privés
 * *************************************/
private:
        /**
         * @brief camera : objet permettant l'inteface avec la caméra
         */
    Pylon::CInstantCamera *camera;

        /**
         * @brief cvImage : image récupérée de la caméra convertie au format OpenCV
         */
    cv::Mat cvImage;

        /**
         * @brief Dimensions voulues pour la capture des image par la caméra
         */
    uint im_width, im_height;
};

#endif // CAMERA_SUPPORT_H
