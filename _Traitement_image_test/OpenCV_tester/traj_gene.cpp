#include "traj_gene.h"

using namespace cv;
using namespace std;

/// <summary>
/// Fonction permettant de générer les trajectoires en fonction de l'image et du squelette
/// </summary>
/// <param name="image"></param>
/// <param name="skeleton"></param>
/// <param name="trajectories"></param>
void trajectoryGeneration(Mat image, Mat skeleton, std::vector<std::vector<Point>>* trajectories)
{
	printSeparator("Debut de calcul des trajectoires.");
	Mat region(image.size(), CV_8UC1, Scalar(0));

	cout << endl << "\tDetections des regions..." << endl;
	int nbRegion = getRegion(image.clone(), &region);
	cout << "\tNombre de regions detectees : " << nbRegion << endl;

	cout << "\tLabelisation du squelette..." << endl;

	Mat skeletonLabelised(skeleton.size(), CV_8UC1, Scalar(0));

	pointsInRegion(skeleton, region, skeletonLabelised);

	cout << "\tCalcul des trajectoires par region..." << endl;

	Mat tracedSkeletonPoints(skeleton.size(), CV_8UC1, Scalar(0));

	for (int i = 1; i < nbRegion; i++) // i = 1 car on en prend pas le background 0
	{
		trajectoryInRegion(skeletonLabelised, i, tracedSkeletonPoints, trajectories);
	}

	printSeparator("Fin de calcul des trajectoires.");
}

/// <summary>
/// Extrait les différentes régions de l'image
/// </summary>
/// <param name="img"></param>
/// <param name="region"></param>
/// <returns></returns>
int getRegion(Mat img, Mat* region)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
		}
	}

	Mat labelImage(img.size(), CV_32S);
	int nLabels = connectedComponents(img, labelImage, 8);


	for (int r = 0; r < region->rows; ++r) {
		for (int c = 0; c < region->cols; ++c) {
			region->at<uchar>(r, c) = labelImage.at<int>(r, c);
		}
	}

	return nLabels;
}

/// <summary>
/// Permet de labeliser le squelette.
/// </summary>
/// <param name="skeleton"></param>
/// <param name="region"></param>
/// <param name="result"></param>
void pointsInRegion(Mat skeleton, Mat region, Mat result)
{
	for (int i = 0; i < skeleton.rows; i++)
	{
		for (int j = 0; j < skeleton.cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == 255)
				result.at<uchar>(i, j) = region.at<uchar>(i, j);
		}
	}

}

/// <summary>
/// Calcul les trajectoires nécessaires pour couvrir la région
/// </summary>
/// <param name="skeleton"></param>
/// <param name="regionNbr"></param>
/// <param name="pointsDone"></param>
/// <param name="trajectories"></param>
void trajectoryInRegion(Mat skeleton, int regionNbr, Mat pointsDone, vector<vector<Point>>* trajectories)
{
	int nbrPointInRegion = 0;

	vector<vector<Point>> trajInRegion;

	for (int i = 0; i < skeleton.rows; i++) //Compte du nombre de pixels du squelette dans la region
	{
		for (int j = 0; j < skeleton.cols; j++)
		{
			if (skeleton.at<uchar>(i, j) == regionNbr)
				nbrPointInRegion++;
		}
	}

	cout << "\t\tPoints dans la region " << regionNbr << " : " << nbrPointInRegion << endl;

	Point currentPoint(0, 0);
	Point previousPoint(0, 0);
	int i, j;
	bool pointFound = false;

	vector<Point> currentTraj;

	bool trajEnded = false;

	Point firstDonePoint(0, 0);
	Point firstUndonePoint(0, 0);
	const Point pointZero(0, 0);
	bool premierPassage = true;

	bool errorTrajectory = false;


	do
	{
		currentTraj.clear();
		currentPoint = pointZero;
		pointFound = false;

		//Recherche du premier point non fait de la region si c'est le premier passage
		for (i = 0; i < skeleton.rows; i++)
		{
			for (j = 0; j < skeleton.cols; j++)
			{
				if (skeleton.at<uchar>(i, j) == regionNbr &&
					pointsDone.at<uchar>(i, j) == 0 &&
					premierPassage)
				{
					currentPoint = Point(j, i);
					pointsDone.at<uchar>(i, j) = 255;
					pointFound = true;
					break;
				}
				else if (skeleton.at<uchar>(i, j) == regionNbr &&
					pointsDone.at<uchar>(i, j) == 255 &&
					!premierPassage)
				{
					currentPoint = Point(j, i);
					pointsDone.at<uchar>(i, j) = 255;
					pointFound = true;
					break;
				}
			}
			if (pointFound)
				break;
		}

		if (premierPassage)
		{
			currentTraj.push_back(currentPoint);
			nbrPointInRegion--;
		}

		trajEnded = false;

		//Trouvons les prochains points (clockwise)
		do
		{
			firstDonePoint = pointZero;
			firstUndonePoint = pointZero;

			Point currentPosition;
			int compteur = 1;

			do
			{
				//On utilise 4 fois le même type d'algo pour parcourir les 4 bords
				//Limite supérieure
				for (int i = -(POINT_SPACING + compteur); i <= (POINT_SPACING + compteur); i++)
				{
					currentPosition = Point(currentPoint.x + (POINT_SPACING + 1), currentPoint.y + i);
					if (skeleton.at<uchar>(currentPosition) == regionNbr &&
						previousPoint != currentPosition)
					{
						if (pointsDone.at<uchar>(currentPosition) == 255 &&
							firstDonePoint == pointZero) //Premier point non deja fait
						{
							firstDonePoint = currentPosition;
							if (firstUndonePoint != pointZero) //Si on a nos deux first, on break la loop
								break;
						}
						else if (pointsDone.at<uchar>(currentPosition) == 0 &&
							firstUndonePoint == pointZero) //Premier point deja fait
						{
							firstUndonePoint = currentPosition;
							if (firstDonePoint != pointZero) //Si on a nos deux first, on break la loop
								break;
						}
					}
				}

				//Limite Droite
				if (firstDonePoint == pointZero || firstUndonePoint == pointZero)
				{
					for (int i = (POINT_SPACING + compteur); i >= -(POINT_SPACING + compteur); i--)
					{
						currentPosition = Point(currentPoint.x + i, currentPoint.y + (POINT_SPACING + 1));
						if (skeleton.at<uchar>(currentPosition) == regionNbr &&
							previousPoint != currentPosition)
						{
							if (pointsDone.at<uchar>(currentPosition) == 255 &&
								firstDonePoint == pointZero) //Premier point non deja fait
							{
								firstDonePoint = currentPosition;
								if (firstUndonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
							else if (pointsDone.at<uchar>(currentPosition) == 0 &&
								firstUndonePoint == pointZero) //Premier point deja fait
							{
								firstUndonePoint = currentPosition;
								if (firstDonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
						}
					}
				}

				//Limite basse
				if (firstDonePoint == pointZero || firstUndonePoint == pointZero)
				{
					for (int i = (POINT_SPACING + compteur); i >= -(POINT_SPACING + compteur); i--)
					{
						currentPosition = Point(currentPoint.x - (POINT_SPACING + 1), currentPoint.y + i);
						if (skeleton.at<uchar>(currentPosition) == regionNbr &&
							previousPoint != currentPosition)
						{
							if (pointsDone.at<uchar>(currentPosition) == 255 &&
								firstDonePoint == pointZero) //Premier point non deja fait
							{
								firstDonePoint = currentPosition;
								if (firstUndonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
							else if (pointsDone.at<uchar>(currentPosition) == 0 &&
								firstUndonePoint == pointZero) //Premier point deja fait
							{
								firstUndonePoint = currentPosition;
								if (firstDonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
						}
					}
				}

				//Limite gauche
				if (firstDonePoint == pointZero || firstUndonePoint == pointZero)
				{
					for (int i = -(POINT_SPACING + compteur); i <= (POINT_SPACING + compteur); i++)
					{
						currentPosition = Point(currentPoint.x + i, currentPoint.y - (POINT_SPACING + 1));
						if (skeleton.at<uchar>(currentPosition) == regionNbr &&
							previousPoint != currentPosition)
						{
							if (pointsDone.at<uchar>(currentPosition) == 255 &&
								firstDonePoint == pointZero) //Premier point non deja fait
							{
								firstDonePoint = currentPosition;
								if (firstUndonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
							else if (pointsDone.at<uchar>(currentPosition) == 0 &&
								firstUndonePoint == pointZero) //Premier point deja fait
							{
								firstUndonePoint = currentPosition;
								if (firstDonePoint != pointZero) //Si on a nos deux first, on break la loop
									break;
							}
						}
					}
				}

				compteur--;
			}while (firstDonePoint == pointZero && firstUndonePoint == pointZero && compteur > - POINT_SPACING + 2);
			//Détermination du prochain point de la trajectoire
			if (premierPassage)
			{
				if (firstDonePoint != pointZero)
				{
					currentTraj.push_back(firstDonePoint);
					trajEnded = true;
				}
				else if (firstUndonePoint != pointZero)
				{
					previousPoint = currentPoint;
					currentPoint = firstUndonePoint;

					pointsDone.at<uchar>(firstUndonePoint) = 255;

					currentTraj.push_back(firstUndonePoint);

					//suppressionVoisinnage(&skeleton, previousPoint, &nbrPointInRegion, POINT_SPACING - compteur);

					nbrPointInRegion--;
				}
				else
				{
					trajEnded = true;
				}
			}
			else
			{
				if (firstUndonePoint != pointZero)
				{
					currentTraj.push_back(currentPoint);

					previousPoint = currentPoint;
					currentPoint = firstUndonePoint;

					nbrPointInRegion--;

					pointsDone.at<uchar>(currentPoint) = 255;

					currentTraj.push_back(currentPoint);

					//suppressionVoisinnage(&skeleton, previousPoint, &nbrPointInRegion, POINT_SPACING + 1 - compteur);

					premierPassage = true;

				}
				else if (firstDonePoint != pointZero)
				{
					previousPoint = currentPoint;
					currentPoint = firstDonePoint;
				}
				else
				{
					errorTrajectory = true;
					cout << "\t\t" << "Erreur dans la detection de trajectoire... " << nbrPointInRegion << "points non traités" << endl;
				}
			}

			showCurrentPoint(skeleton, currentPoint);

		} while (!trajEnded && !errorTrajectory);

		trajInRegion.push_back(currentTraj);

		premierPassage = false;

		cout << "\t\t\t" << trajInRegion.size() << " trouvees dans cette region." << endl;

	} while (nbrPointInRegion > 0 && !errorTrajectory);

	trajectories->insert(trajectories->end(), trajInRegion.begin(), trajInRegion.end());
}

void suppressionVoisinnage(Mat* skeleton, Point point, int* ptNbr, int dist)
{
	bool erase = false;
	bool c1, c2, c3, c4, c5, c6;

	for (int x = -dist; x <= dist; x++)
	{
		for (int y = -dist; y <= dist; y++)
		{
			c1 = point.x + x > 0;
			c2 = point.x + x < skeleton->cols;
			c3 = point.y + y > 0;
			c4 = point.y + y < skeleton->rows;
			c5 = x != y;
			c6 = x != 0;

			erase = c1 &&
				c2 &&
				c3 &&
				c4 &&
				(c5 || c6);

			if (erase)
			{
				if (skeleton->at<uchar>(point.y + x, point.x + y) > 0)
				{
					skeleton->at<uchar>(point.y + x, point.x + y) = 0;
					if (ptNbr != NULL) (*ptNbr)--;
				}

			}
		}
	}
}


