
#include "zhang-suen.h"

using namespace cv;
using namespace std;


/// <summary>
/// Fonction permettant d'extraire un squelette morphologique d'une forme à partir de l'algorithme de Zhang-Suen
/// </summary>
/// <param name="image"></param>
/// <returns></returns>
int zhang_suen_process(Mat image)
{
	/***************************************************************
								Variables
	****************************************************************/
	Mat image2 = image.clone();
	Mat skeleton = image.clone();

	//Timer
	clock_t startAll;
	double durationAll;


	/***************************************************************
								Seuillage
	****************************************************************/
	threshold(skeleton, skeleton, 100, 255, THRESH_BINARY);


	/***************************************************************
						   Début de l'algorithme
	****************************************************************/
	startAll = clock();
	zhang_suen_algorithm(skeleton);


	/***************************************************************
						Arret et affichage du timer
	****************************************************************/
	durationAll = ((double)clock() - (double)startAll) / (double)CLOCKS_PER_SEC;

	std::cout << std::endl << std::endl << durationAll << " s" << std::endl << std::endl;


	/***************************************************************
						   Affichage du résultat
	****************************************************************/
	const int blackRatio = 8;

	//Négatif de l'image puis réhausse du niveau du noir (pour plus de lisibilité)
	for (int i = 0; i < image2.rows; i++)
	{
		for (int j = 0; j < image2.cols; j++)
		{
			image2.at<uchar>(i, j) = 255 - image2.at<uchar>(i, j);
			image2.at<uchar>(i, j) = (image2.at<uchar>(i, j) + (blackRatio - 1) * 255) / blackRatio;
		}
	}

	//Conversion de l'image en couleur
	cvtColor(image2, image2, COLOR_GRAY2BGR);

	//Affichage du squelette sur l'image
	for (int i = 0; i < image2.rows; i++)
	{
		for (int j = 0; j < image2.cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 0)
				image2.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
		}
	}

	imshow("Skeleton ", image2);

	waitKey(0);

	return 0;
}
/// <summary>
/// Fonction effectuant l'algorithme de Zhang-Suen sur l'image passée en paramètre
/// </summary>
/// <param name="image"></param>
void zhang_suen_algorithm(Mat skeleton)
{
	int i, j;
	const int rows = skeleton.rows;
	const int cols = skeleton.cols;

	int compteur = 0;

	Mat image = skeleton.clone();

	Mat markedForDeletion;

	Rect neighbourhood(0, 0, 3, 3);

	do
	{
		//RAZ du tableau de pixels à supprimer
		markedForDeletion = Mat(skeleton.size(), CV_8UC1, Scalar(0));

		for (i = 1; i < rows - 1; i++) //Parcours de l'image (les pixels du bords ne peuvent pas être marqués à cause de la condition 1)
		{
			for (j = 1; j < cols - 1; j++)
			{
				if (skeleton.at<uchar>(i, j) == 0) //Condition 1
				{
					if (skeleton.at<uchar>(i, j + 1) == 255 || skeleton.at<uchar>(i - 1, j) == 255 || skeleton.at<uchar>(i + 1, j) == 255) //Pass 1 condition 3
					{
						if (skeleton.at<uchar>(i - 1, j) == 255 || skeleton.at<uchar>(i + 1, j) == 255 || skeleton.at<uchar>(i, j - 1) == 255) //Pass 1 condition 4
						{
							neighbourhood.y = i - 1;
							neighbourhood.x = j - 1;

							if (conditions3puis4(skeleton(neighbourhood)))
								markedForDeletion.at<uchar>(i, j) = 255;
						}
					}
					else if (skeleton.at<uchar>(i - 1, j) == 255 || skeleton.at<uchar>(i, j + 1) == 255 || skeleton.at<uchar>(i + 1, j) == 255) //Pass 2 condition 3
					{
						if (skeleton.at<uchar>(i, j + 1) == 255 || skeleton.at<uchar>(i + 1, j) == 255 || skeleton.at<uchar>(i, j - 1) == 255) //Pass 2 condition 4
						{
							neighbourhood.y = i - 1;
							neighbourhood.x = j - 1;

							if (conditions3puis4(skeleton(neighbourhood)))
								markedForDeletion.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}

		for (i = 1; i < rows - 1; i++) //Parcours de l'image (les pixels du bords ne peuvent pas être marqués à cause de la condition 1)
		{
			for (j = 1; j < cols - 1; j++)
			{
				if (markedForDeletion.at<uchar>(i, j))
					skeleton.at<uchar>(i, j) = 255;
			}
		}


		compteur = countNonZero(markedForDeletion);

		/*imshow("Squelette", skeleton);
		waitKey(0);

		std::cout << "Nombre de pixels restant : " << compteur << std::endl;*/
	} while (compteur > 0);
}

/// <summary>
/// Vérifie les conditions  3 puis 4 de l'algorithme de Zhang-Suen
/// </summary>
/// <param name="neighbourhood"></param>
/// <returns></returns>
bool conditions3puis4(Mat neighbourhood)
{
	int nbrBlackNeighbour;

	nbrBlackNeighbour = blackNeighbour(neighbourhood);

	if (nbrBlackNeighbour <= 6 && nbrBlackNeighbour >= 2) //Condition 2
	{
		if (connectivites(neighbourhood) == 1)
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

/// <summary>
/// Fonction déterminant le nombre de pixels noir dans le voisinnage du pixel central de l'image 3x3 envoyée.
/// </summary>
/// <param name="neighbourhood"></param>
/// <returns></returns>
int blackNeighbour(Mat neighbourhood)
{
	int nbr = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (neighbourhood.at<uchar>(i, j) == 0)
			{
				nbr++;
			}
		}
	}

	nbr--;

	return nbr;
}

/// <summary>
/// Indique le nombre de connectivites du pixel central dans l'image 3x3 envoyée en paramètre
/// </summary>
/// <param name="neighbourhood"></param>
/// <returns></returns>
int connectivites(Mat neighbourhood)
{
	int i = 1, j = 1;
	int nbrConnectivities = 0;

	if (!neighbourhood.at<uchar>(i - 1, j - 1))
	{
		if (neighbourhood.at<uchar>(i - 1, j))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i - 1, j))
	{
		if (neighbourhood.at<uchar>(i - 1, j + 1))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i - 1, j + 1))
	{
		if (neighbourhood.at<uchar>(i, j + 1))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i, j + 1))
	{
		if (neighbourhood.at<uchar>(i + 1, j + 1))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i + 1, j + 1))
	{
		if (neighbourhood.at<uchar>(i + 1, j))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i + 1, j))
	{
		if (neighbourhood.at<uchar>(i + 1, j - 1))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i + 1, j - 1))
	{
		if (neighbourhood.at<uchar>(i, j - 1))
			nbrConnectivities++;
	}
	if (!neighbourhood.at<uchar>(i, j - 1))
	{
		if (neighbourhood.at<uchar>(i - 1, j - 1))
			nbrConnectivities++;
	}

	return nbrConnectivities;
}

