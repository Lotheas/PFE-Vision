
#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>

#include <opencv2/opencv.hpp>

#include "traj_gene.h"
#include "skeleton.h"
#include "utilitaire.h"

#define SCALE 3
#define DOWNSCALE true

using namespace cv;
using namespace std;

vector<vector<Point>> trajectory_generation(Mat image)
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
		resize(image, image, Size(image.size().width * 0.5, image.size().height * 0.5), 0.5, 0.5, INTER_NEAREST_EXACT);
	}

	showCurrentPoint(image, Point(0, 0), SCALE);

	Mat skeleton = image.clone();

	printVoidSeparator('*');

	//sqeulettisation
	morphological_skeleton(&skeleton);

	showCurrentPoint(skeleton, Point(0, 0), SCALE);
	
	printVoidSeparator('*');

	vector<vector<Point>> trajectoires;

	trajectoryGeneration(image, skeleton, false, &trajectoires);

	printVoidSeparator('*');

	drawTrajectories(skeleton, trajectoires, 7, SCALE);

	//On unifie les trajectoires pour avoir des trajectoires continues
	vector<vector<Point>> unifiedTrajectories;

	printSeparator("Unification des trajectoires");
	fusionTrajectories(trajectoires, &unifiedTrajectories, 5);

	drawTrajectories(skeleton, unifiedTrajectories, 7, SCALE);

	//On allège la trajectoire
	vector<vector<Point>> lighterTrajectories;

	printSeparator("Allegement des trajectoires");
	lightenTrajectories(unifiedTrajectories, &lighterTrajectories, 10);

	drawTrajectories(skeleton, lighterTrajectories, 1, SCALE);

    return lighterTrajectories;
}
