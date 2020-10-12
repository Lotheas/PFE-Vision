#include "camera_support.h"

using namespace Pylon;

camera_support::camera_support()
{
    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();

    //Set default value
    im_width = 3088;
    im_height = 2064;
}

camera_support::~camera_support()
{
    if(camera != NULL)
        delete camera;

    PylonTerminate();
}

void camera_support::initilialize()
{
    try {
        camera = new CInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());

        camera->Open();

        GenApi::INodeMap& nodemap = camera->GetNodeMap();

        //Objects permettant l'accès au paramètres
        CEnumParameter iPixelFormat(nodemap, "PixelFormat");
        CIntegerParameter iWidth(nodemap, "Width");
        CIntegerParameter iHeight(nodemap, "Height");
        CIntegerParameter iOffsetX(nodemap, "OffsetX");
        CIntegerParameter iOffsetY(nodemap, "OffsetY");
        qInfo() << "Paramètrage de la caméra : ";
        iPixelFormat.SetValue("BGR8");
        iWidth.SetValue(im_width ,IntegerValueCorrection_Nearest);
        iHeight.SetValue(im_height, IntegerValueCorrection_Nearest);
        iOffsetX.SetValue((3088 - im_width) / 2, IntegerValueCorrection_Nearest);
        iOffsetY.SetValue((2064 - im_height) / 2, IntegerValueCorrection_Nearest);

        qInfo() << "\tWidth : " << iWidth.GetValue() << "\t(Target : " << im_width << ")";
        qInfo() << "\tHeight : " << iHeight.GetValue() << "\t(Target : " << im_height << ")";
        qInfo() << "\tOffsetX : " << iOffsetX.GetValue() << "\t(Target : " << (3088 - im_width) / 2 << ")";
        qInfo() << "\tOffsetY : " << iOffsetY.GetValue() << "\t(Target : " << (2064 - im_height) / 2 << ")";

    } catch (const GenericException *e) {
        std::cout << "erreur : " << e->GetDescription();
    } catch (const std::exception e) {
        QMessageBox msg;
        msg.setText("Exception!!");
        msg.exec();
    }
}

bool camera_support::take_picture(cv::Mat *result)
{
    camera->Close();

    camera->Open();
    camera->StartGrabbing(); //Constructor with default parameters
qInfo() << "\tgrab started.";
    CGrabResultPtr ptrGrabResult;

    camera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

    if(ptrGrabResult->GrabSucceeded())
    {
        CImageFormatConverter converter;
        //Indique le format de l'image de sortie du convertisseur
        converter.OutputPixelFormat = PixelType_BGR8packed;

        CPylonImage pylonImage;
        //Covnerti l'image au ofrmat voulu
        converter.Convert(pylonImage, ptrGrabResult);
        //Créé l'image au format openCV
        cvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),  CV_8UC3, (uint8_t*) pylonImage.GetBuffer());

//        //Sauvegarde l'image obtenue
//        qInfo() << "\tImage saving...";
//        std::ostringstream s;
//        // Store the image by index definition file name
//        s << "CVimage_" << "0" << ".jpg";
//        std::string imageName(s.str());
//        //Save OpenCV image.
//        imwrite(imageName, cvImage);

        qInfo() << "\tcvImage dimensions : " << cvImage.rows << " x " << cvImage.cols;
        *result = cvImage.clone();
        qInfo() << "\tResult dimensions : " << result->rows << " x " << result->cols;

//        qInfo() << "\tResult saving...";
//        std::ostringstream s2;
//        // Store the image by index definition file name
//        s2 << "result_" << "0" << ".jpg";
//        std::string imageName2(s2.str());
//        //Save OpenCV image.
//        imwrite(imageName2, *result);
//        qInfo() << "\tImage saved. Returning cvImage.";

    }
    else
        qInfo("Grab not succeded");

    if(ptrGrabResult->GrabSucceeded())
        return true;

    return false;
}

void camera_support::change_resolution(uint _width, uint _height)
{
    im_width = _width;
    im_height = _height;
    //Le changement se faire lors de l'initialisation
}

void camera_support::unInitialize()
{
    if(camera->IsGrabbing())
        camera->StopGrabbing();
    if(camera->IsOpen())
        camera->Close();
}

