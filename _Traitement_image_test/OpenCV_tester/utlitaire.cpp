#include "utilitaire.h"

using namespace std;
using namespace cv;



void printSeparator(string title)
{
	int nbrSpace = 5;
	int nbrEqual = SEPARATOR_LENGTH - title.length() - 2 * nbrSpace;

	string affichage;

	for (int i = 0; i < nbrEqual / 2; i++)
	{
		affichage += "=";
	}

	for (int i = 0; i < nbrSpace; i++)
	{
		affichage += " ";
	}

	affichage += title;

	for (int i = 0; i < nbrSpace; i++)
	{
		affichage += " ";
	}

	for (int i = 0; i < nbrEqual / 2; i++)
	{
		affichage += "=";
	}

	if (title.length() % 2 == 1)
		affichage += "=";

	cout << endl << affichage << endl;
}

void printVoidSeparator(char character)
{
	string affichage;

	for (int i = 0; i < SEPARATOR_LENGTH; i++)
	{
		affichage += character;
	}

	cout << endl << endl << affichage << endl;
}


void showCurrentPoint(Mat skeleton, Point pt)
{
	Mat toprint(skeleton.size(), CV_8UC3, Scalar(0));

	for (int i = 0; i < skeleton.rows; i++)
	{
		for (int j = 0; j < skeleton.cols; j++)
		{
			if (skeleton.at<uchar>(i, j))
			{
				toprint.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
		}
	}

	circle(toprint, pt, 10, Vec3b(0, 0, 255));

	imshow("Current Point", toprint);
	waitKey(0);
}