#include "ouv_morpho.h"

using namespace cv;
using namespace std;


/// <summary>
/// Fonction implémentant une algorithme d'amincissement par ouverture morhpologique
/// </summary>
/// <param name="image"></param>
/// <returns></returns>
int morphological_skeleton(Mat* image)
{


	//Negatif
	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			image->at<uchar>(i, j) = 255 - image->at<uchar>(i, j);
		}
	}

	Mat image2 = image->clone();

	const int blackRatio = 8;

	for (int i = 0; i < image2.rows; i++)
	{
		for (int j = 0; j < image2.cols; j++)
		{
			image2.at<uchar>(i, j) = (image2.at<uchar>(i, j) + (blackRatio - 1) * 255) / blackRatio;
		}
	}

	Mat image3 = image2.clone();

	clock_t start = clock();
	//Morphological skeleton
	Mat skeleton(image->size(), CV_8UC1);
	Mat tampon;
	Mat eroded;

	const int elem_size = 3;
	printSeparator("Debut de l'algorithme de squelettisation.");
	cout << endl;

	Mat element = cv::getStructuringElement(cv::MORPH_OPEN, cv::Size(elem_size, elem_size));

	cout << "\tBinarisation de l'image..." << endl;
	threshold(*image, *image, 12, 255, THRESH_BINARY);

	cout << "\tExtraction du squelette..." << endl;
	bool done;
	do
	{
		morphologyEx(*image, tampon, MORPH_OPEN, element);
		subtract(*image, tampon, tampon);
		bitwise_or(skeleton, tampon, skeleton);
		erode(*image, *image, element);

		done = (countNonZero(*image) == 0);
	} while (!done);

	cvtColor(image2, image2, COLOR_GRAY2BGR);

	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
				image2.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
		}
	}

	std::cout << "\tSquelette extrait. Temps ecoule :  " << ((double)clock() - (double)start) / (double)CLOCKS_PER_SEC << " secondes" << endl;

	imshow("Skeleton basic", image2);
	waitKey(0);


	// *********************************************
	//
	// Retraitement de l'image
	//
	// *********************************************
	cout << "\tSuppression des points \"parasites\"..." << endl;
	removeLonelyPoint(skeleton);

	cout << "\tEspacement des points..." << endl;
	spacePoint(skeleton);

	// *********************************************
	//
	// Affichage de l'image
	//
	// *********************************************
	cvtColor(image3, image3, COLOR_GRAY2BGR);

	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
				image3.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
		}
	}

	//imshow("Skeleton avec suppression des pts isolés", image3);

	*image = Mat(image->size(), CV_8UC1, Scalar(0));
	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
				image->at<uchar>(i, j) = 255;
		}
	}
	/*imshow("Image basics", *image);

	waitKey(0);*/

	destroyAllWindows();

	printSeparator("Fin de l'algorithme de squelettisation.");

	return 0;
}

/// <summary>
/// Supprime les points seuls de la trajectoire
/// </summary>
/// <param name="image"></param>
void removeLonelyPoint(Mat skeleton)
{
	bool toBeDeleted = false;

	//Suppression des points isolés
	for (int i = 0; i < skeleton.rows; i++)
	{
		for (int j = 0; j < skeleton.cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
			{
				toBeDeleted = true;

				for (int x = -1; x <= 1; x++)
				{
					for (int y = -1; y <= 1; y++)
					{
						if (x + i >= 0 &&
							x + i < skeleton.rows &&
							y + j >= 0 && y + j < skeleton.cols &&
							(x != 0 || y != 0))
						{
							if (skeleton.at<uchar>(i + x, j + y) == 255)
								toBeDeleted = false;
						}
					}
				}

				//Supprime le pixel si aucun autre pixel n'est dnas le voisinnage proche
				if (toBeDeleted)
					skeleton.at<uchar>(i, j) = 0;
			}
		}
	}
}

/// <summary>
/// Supprimer certaines points du squelette pour les espacer
/// </summary>
/// <param name="image"></param>
void spacePoint(Mat skeleton)
{
	int spacing = POINT_SPACING; 

	for (int i = 0; i < skeleton.rows; i++)
	{
		for (int j = 0; j < skeleton.cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
			{
				for (int x = -spacing; x <= spacing; x++)
				{
					for (int y = -spacing; y <= spacing; y++)
					{
						if (i + x > 0 &&
							i + x < skeleton.rows &&
							j + y > 0 &&
							j + y < skeleton.cols &&
							(x != y || x != 0))
						{
							skeleton.at<uchar>(i + x, j + y) = 0;
						}
					}
				}
			}
		}
	}
}


