
#include <iostream>
#include <cstdio>
#include <ctime>

#include <thread>

#include <opencv2/opencv.hpp>


#include "traj_gene.h"
#include "zhang-suen.h"
#include "stentiford.h"
#include "ouv_morpho.h"
#include "utilitaire.h"



using namespace cv;
using namespace std;

int main()
{
	std::cout << "Hello World!" << std::endl;
	Mat image = imread("C:/Users/bazoo/source/repos/OpenCV_tester/img.png", IMREAD_GRAYSCALE);

	// Check for failure
	if (image.empty())
	{
		std::cout << "Could not open or find the image" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	Mat skeleton = image.clone();

	printVoidSeparator('*');

	//sqeulettisation
	morphological_skeleton(&skeleton);

	imshow("Skeleton", skeleton);
	waitKey(0);

	printVoidSeparator('*');

	//Déclaration des différentes trajectoirs
	vector<vector<Point>> trajectoire;

	//Calcul des trajectoirs
	trajectoryGeneration(image.clone(), skeleton.clone(), &trajectoire);

	printVoidSeparator('*');

	return 0;
}
