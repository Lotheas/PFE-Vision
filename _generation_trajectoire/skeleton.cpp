#include "skeleton.h"

using namespace cv;
using namespace std;


/// <summary>
/// Fonction implémentant un algorithme  de squelettisation par amincissement
/// </summary>
/// <param name="image"></param>
/// <returns></returns>
void morphological_skeleton(Mat* image)
{
	//Negatif
	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			image->at<uchar>(i, j) = 255 - image->at<uchar>(i, j);
		}
	}

	//Sauvegarde de l'image de base pour l'affichage post-algorithme
	Mat image2 = image->clone();

	const int blackRatio = 8;

	for (int i = 0; i < image2.rows; i++)
	{
		for (int j = 0; j < image2.cols; j++)
		{
			image2.at<uchar>(i, j) = (image2.at<uchar>(i, j) + (blackRatio - 1) * 255) / blackRatio;
		}
	}

	printSeparator("Debut de l'algorithme de squelettisation.");
	cout << endl;

	clock_t start = clock();
	//Morphological skeleton
	cout << "\tBinarisation de l'image..." << endl;
	threshold(*image, *image, 12, 255, THRESH_BINARY);
	Mat skeleton = image->clone();

	const int elem_size = 3;
	

	const int familySize = 8;

	//Initialization des éléments structurants de la famille Mskel
	Mat* mskel = new Mat[familySize];

	for (int i = 0; i < familySize; i++)
	{
		mskel[i] = Mat(Size(3, 3), CV_8U, Scalar(0));
	}

	//On définit l'élément de base connexité 8 (en négatif)
	// 0	= noir
	// 127	= X (no care)
	// 255	= blanc
	mskel[0].at<uchar>(0, 0) = 0;
	mskel[0].at<uchar>(0, 1) = 0;
	mskel[0].at<uchar>(0, 2) = 0;
	mskel[0].at<uchar>(1, 0) = 127;
	mskel[0].at<uchar>(1, 1) = 255;
	mskel[0].at<uchar>(1, 2) = 127;
	mskel[0].at<uchar>(2, 0) = 255;
	mskel[0].at<uchar>(2, 1) = 255;
	mskel[0].at<uchar>(2, 2) = 255;

	mskel[1].at<uchar>(0, 0) = 0;
	mskel[1].at<uchar>(0, 1) = 0;
	mskel[1].at<uchar>(0, 2) = 127;
	mskel[1].at<uchar>(1, 0) = 0;
	mskel[1].at<uchar>(1, 1) = 255;
	mskel[1].at<uchar>(1, 2) = 255;
	mskel[1].at<uchar>(2, 0) = 127; //Pour connexité 4, jsute remplacer celui-là par 255, 127 pour 8 connexité
	mskel[1].at<uchar>(2, 1) = 255;
	mskel[1].at<uchar>(2, 2) = 127;

	////affichage de mskel
	//cout << "Mskel 1 :" << endl;
	//matrixVisual(mskel[0]);

	////affichage de mskel
	//cout << "Mskel 2 :" << endl;
	//matrixVisual(mskel[1]);

	//Construction des autres matrices par rotation de 45°
	for (int i = 1; i < familySize / 2; i++)
	{
		mskel[i * 2] = rotate45(rotate45(mskel[i * 2 - 2]));

		mskel[i * 2 + 1] = rotate45(rotate45(mskel[i * 2 - 2 + 1]));

		//Affichage des mskel
		/*cout << "Mskel " << i * 2 + 1 << " :" << endl;
		matrixVisual(mskel[i * 2]);
		cout << "Mskel " << i * 2 + 2 << " :" << endl;
		matrixVisual(mskel[i * 2 + 1]);*/
	}

	cout << "\tExtraction du squelette..." << endl;
	//int count = 0;
	bool done;
	do
	{
		Mat ref = skeleton.clone();

		//count++;
		for (int i = 0; i < familySize; i++)
		{
			skeleton = amincissement(skeleton, mskel[i]);
		}

		Mat diff;
		subtract(ref, skeleton, diff);

		/*Mat diffUp;
		resize(diff, diffUp, Size(diff.size().width * 2, diff.size().height * 2), 2, 2, INTER_LINEAR_EXACT);
		imshow("Skeleton Temp", diffUp);
		waitKey(0);*/

		done = (countNonZero(diff) == 0);
	} while (!done);

	std::cout << "\tSquelette extrait. Temps ecoule :  " << ((double)clock() - (double)start) / (double)CLOCKS_PER_SEC << " secondes" << endl;

	cvtColor(image2, image2, COLOR_GRAY2BGR);

	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
				image2.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
		}
	}

	/*Mat img2Up;
	resize(image2, img2Up, Size(image2.size().width * 2, image2.size().height * 2), 2, 2, INTER_NEAREST_EXACT);
	imshow("Skeleton basic", img2Up);
	waitKey(0);*/

	*image = skeleton.clone();

	delete[] mskel;


	printSeparator("Fin de l'algorithme de squelettisation.");
}

/// <summary>
/// Réalise un amincissement de l'image par l'élément structurant 
/// </summary>
/// <param name="image"></param>
/// <param name="element"></param>
/// <returns></returns>
Mat amincissement(Mat image, Mat element)
{
	Mat ret = image.clone();

	vector<Point> pointsToRemove;

	uint8_t elem;
	bool toRem;

	//On sélectionne les points à supprimer
	//Parcours de l'image
	for (int x = 1; x < image.rows - 1; x++)
	{
		for (int y = 1; y < image.cols - 1; y++)
		{
			toRem = true;
			//Parcours du voisinnage du point
			for (int xl = -1; xl <= 1; xl++)
			{
				for (int yl = -1; yl <= 1; yl++)
				{
					/*try
					{*/

					elem = element.at<uchar>(xl + 1, yl + 1);
					/*}
					catch (const std::exception&)
					{
						cout << endl << "element.at" << endl;
					}*/
					/*try
					{*/

					if (elem != 127 && (elem != image.at<uchar>(x + xl, y + yl)))
						toRem = false;
					/*}
					catch (const std::exception&)
					{
						cout << endl << "image.at" << endl;
					}*/

					if (!toRem)
						break;
				}
				if (!toRem)
					break;
			}

			if (toRem)
				pointsToRemove.push_back(Point(y, x));
		}
	}
	//cout << "Nombre de points à supprimer : " << pointsToRemove.size() << endl;
	//On supprime les points sélectionnés
	for (int i = 0; i < pointsToRemove.size(); i++)
	{

		/*try
		{*/

		ret.at<uchar>(pointsToRemove[i]) = 0;
		/*}
		catch (const std::exception&)
		{
			cout << endl << "ret.at" << endl;
		}*/
	}

	return ret.clone();
}

/// <summary>
/// Rotation de 45° autour du centre d'un matrice 3x3 sans extrapolation
/// </summary>
/// <param name="_m"></param>
/// <returns></returns>
Mat rotate45(Mat _m)
{
	Mat ret;

	ret = _m.clone();

	ret.at<uchar>(0, 0) = _m.at<uchar>(0, 1);
	ret.at<uchar>(1, 0) = _m.at<uchar>(0, 0);
	ret.at<uchar>(2, 0) = _m.at<uchar>(1, 0);
	ret.at<uchar>(2, 1) = _m.at<uchar>(2, 0);
	ret.at<uchar>(2, 2) = _m.at<uchar>(2, 1);
	ret.at<uchar>(1, 2) = _m.at<uchar>(2, 2);
	ret.at<uchar>(0, 2) = _m.at<uchar>(1, 2);
	ret.at<uchar>(0, 1) = _m.at<uchar>(0, 2);

	return ret.clone();
}

/// <summary>
/// Permet d'afficher la matrice passée en paramètre dans la console
/// </summary>
/// <param name="_m"></param>
void matrixVisual(Mat _m)
{
	string strOut = "";
	for (int i = 0; i < _m.rows; i++)
	{
		for (int j = 0; j < _m.cols; j++)
		{
			strOut += "\t";
			strOut += to_string(_m.at<uchar>(i, j));
		}
		strOut += "\n";
	}
	cout << strOut << endl;
}

