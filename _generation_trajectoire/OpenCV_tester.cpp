
#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>

#include <opencv2/opencv.hpp>

#include "traj_gene.h"
#include "skeleton.h"
#include "utilitaire.h"



using namespace cv;
using namespace std;

int main()
{
	std::cout << "Hello World!" << std::endl;
	Mat image = imread("C:/Users/bazoo/source/repos/OpenCV_tester - Copy/img.png", IMREAD_GRAYSCALE);

	// Check for failure
	if (image.empty())
	{
		std::cout << "Could not open or find the image" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	showCurrentPoint(image, Point(0, 0), 3);

	Mat skeleton = image.clone();

	printVoidSeparator('*');

	//sqeulettisation
	morphological_skeleton(&skeleton);

	showCurrentPoint(skeleton, Point(0, 0), 3);
	
	printVoidSeparator('*');

	vector<vector<Point>> trajectoires;

	trajectoryGeneration(image, skeleton, true, &trajectoires);

	printVoidSeparator('*');

	drawTrajectories(skeleton, trajectoires, 7, 3);

	//On unifie les trajectoires pour avoir des trajectoires continues
	vector<vector<Point>> unifiedTrajectories;

	printSeparator("Unification des trajectoires");
	fusionTrajectories(trajectoires, &unifiedTrajectories, 5);

	drawTrajectories(skeleton, unifiedTrajectories, 7, 3);

	//On allège la trajectoire
	vector<vector<Point>> lighterTrajectories;

	printSeparator("Allegement des trajectoires");
	lightenTrajectories(unifiedTrajectories, &lighterTrajectories, 10);

	drawTrajectories(skeleton, lighterTrajectories, 1, 3);

	return 0;
}
