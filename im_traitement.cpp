#include "im_traitement.h"

im_traitement::im_traitement()
{

}

using namespace cv;
using namespace std;

vector<vector<Point>> im_traitement::trajectory_generation(Mat image)
{
    //std::cout << "Hello World!" << std::endl;
    //Mat image = imread("C:/Users/bazoo/source/repos/OpenCV_tester - Copy/seuil.bmp", IMREAD_GRAYSCALE);

    // Check for failure
    if (image.empty())
    {
        std::cout << "Could not open or find the image" << std::endl;
        std::cin.get(); //wait for any key press
    }

    if (DOWNSCALE)
    {
        resize(image, image, Size(image.size().width / SCALE, image.size().height * 1 / SCALE), 1 / SCALE, 1 / SCALE, INTER_NEAREST_EXACT);
    }

    //showCurrentPoint(image, Point(0, 0), SCALE);

    Mat skeleton = image.clone();

    printVoidSeparator('*');

    //emit stepChanged(2);

    //sqeulettisation
    morphological_skeleton(&skeleton);

    //showCurrentPoint(skeleton, Point(0, 0), SCALE);

    printVoidSeparator('*');

    vector<vector<Point>> trajectoires;

    trajectoryGeneration(skeleton, false, &trajectoires);

    printVoidSeparator('*');

    //drawTrajectories(skeleton, trajectoires, 7, SCALE);

    //On unifie les trajectoires pour avoir des trajectoires continues
    vector<vector<Point>> unifiedTrajectories;

    printSeparator("Unification des trajectoires");
    fusionTrajectories(trajectoires, &unifiedTrajectories, 5);

    //drawTrajectories(skeleton, unifiedTrajectories, 7, SCALE);

    //On allège la trajectoire
    vector<vector<Point>> lighterTrajectories;

    printSeparator("Allegement des trajectoires");
    lightenTrajectories(unifiedTrajectories, &lighterTrajectories, 10);

    if(DOWNSCALE)
    {
        for (uint i = 0; i < lighterTrajectories.size(); i++)
        {

            for (uint j = 0; j < lighterTrajectories[i].size(); j++)
            {
                lighterTrajectories[i][j].x *= SCALE;
                lighterTrajectories[i][j].y *= SCALE;
            }
        }
    }

    //drawTrajectories(skeleton, lighterTrajectories, 1, SCALE);

    destroyAllWindows();

    return lighterTrajectories;
}

