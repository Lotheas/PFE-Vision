#include "traj_gene.h"

using namespace cv;
using namespace std;



/// <summary>
/// Fonction permettant de générer les trajectoires en fonction de l'image et du squelette
/// </summary>
/// <param name="image"></param>
/// <param name="skeleton"></param>
/// <param name="trajectories"></param>
void trajectoryGeneration(Mat image, Mat skeleton, bool ebranchage, std::vector<std::vector<Point>>* trajectories)
{
	printSeparator("Debut de calcul des trajectoires.");

	//Ebranchage si demandé
	if (ebranchage)
	{
		ebrancher(skeleton);
		showCurrentPoint(skeleton, Point(0, 0), 3);
	}

	//Calcul des trajectoires
	cout << "\tCalcul des trajectoires..." << endl;

	trajectoryInRegion(skeleton, trajectories);

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

void ebrancher(Mat image)
{
	vector<Point> extremites;
	int nbrVoisins;

	//Detection des extrémités
	//Parcours de l'image
	for (int x = 1; x < image.rows - 1; x++)
	{
		for (int y = 1; y < image.cols - 1; y++)
		{
			if (image.at<uchar>(x, y)) //Si on est sur un point du squelette
			{
				nbrVoisins = 0;
				//Parcours du voisinnage
				for (int xl = -1; xl <= 1; xl++)
				{
					for (int yl = -1; yl <= 1; yl++)
					{
						if (image.at<uchar>(x + xl, y + yl))
							nbrVoisins++;
					}
				}
				if (nbrVoisins == 2)
					extremites.push_back(Point(y, x));
			}
		}
	}

	//On affiche les extremites
	/*for (int i = 0; i < extremites.size(); i++)
	{
		showCurrentPoint(image, extremites[i],2);
	}*/

	//On commence l'ébranchage
	for (int i = 0; i < extremites.size(); i++)
	{
		bool done = false;
		Point nextPoint = extremites[i];



		do //Tant qu'on ne retombe pas sur un point avec deux voisins
		{

			//On supprime l'extremité
			image.at<uchar>(nextPoint) = 0;

			bool foundNext = false;
			//Parcours du voisinnage
			for (int xl = -1; xl <= 1; xl++)
			{
				for (int yl = -1; yl <= 1; yl++)
				{
					if (image.at<uchar>(nextPoint.y + xl, nextPoint.x + yl))
					{
						nextPoint = Point(nextPoint.x + yl, nextPoint.y + xl);
						foundNext = true;
					}
					if (foundNext)
						break;
				}
				if (foundNext)
					break;
			}

			//On compte le nombre de voisins du prochain point
			nbrVoisins = 0;
			//Parcours du voisinnage
			for (int xl = -1; xl <= 1; xl++)
			{
				for (int yl = -1; yl <= 1; yl++)
				{
					if (image.at<uchar>(nextPoint.y + xl, nextPoint.x + yl))
						nbrVoisins++;
				}
			}

			if (nbrVoisins != 2)
			{
				done = true;
				image.at<uchar>(nextPoint) = 0;
			}

		} while (!done);
	}

	bitwise_not(image, image);
	morphological_skeleton(&image);

}

/// <summary>
/// Calcul les trajectoires nécessaires pour couvrir la région
/// </summary>
/// <param name="skeleton"></param>
/// <param name="regionNbr"></param>
/// <param name="pointsDone"></param>
/// <param name="trajectories"></param>
void trajectoryInRegion(Mat skeleton, vector<vector<Point>>* trajectories)
{
	Mat pointsDone(skeleton.size(), CV_8UC1, Scalar(0));
	//Représente soit une extrémité soit une point multiple
	vector<ptClef> ptsClefs;
	int nbrVoisins;

	//On trouve les points clefs 
	for (int x = 1; x < skeleton.rows - 1; x++)
	{
		for (int y = 1; y < skeleton.cols - 1; y++)
		{
			if (skeleton.at<uchar>(x, y)) //Si on est sur un point du squelette
			{
				nbrVoisins = 0;
				//Parcours du voisinnage
				for (int xl = -1; xl <= 1; xl++)
				{
					for (int yl = -1; yl <= 1; yl++)
					{
						if (skeleton.at<uchar>(x + xl, y + yl))
							nbrVoisins++;
					}
				}
				if (nbrVoisins != 3)
				{
					ptClef pt;
					pt.pt = Point(y, x);
					ptsClefs.push_back(pt);
				}

			}
		}
	}

	//On affiche les points clefs
	/*for (int i = 0; i < ptsClefs.size(); i++)
	{
		showCurrentPoint(skeleton, ptsClefs[i].pt, 3);
	}*/

	//On défini les points du voisinnage des points clefs
	for (int i = 0; i < ptsClefs.size(); i++)
	{
		for (int xl = -1; xl <= 1; xl++)
		{
			for (int yl = -1; yl <= 1; yl++)
			{
				if (skeleton.at<uchar>(ptsClefs[i].pt.y + xl, ptsClefs[i].pt.x + yl))
				{
					//On vérifie que ce n'est pas un point clef aussi
					bool isMP = false;
					Point currentPt = Point(ptsClefs[i].pt.x + yl, ptsClefs[i].pt.y + xl);
					for (int j = 0; j < ptsClefs.size(); j++)
					{
						isMP = isKeyPointOrNear(currentPt, ptsClefs);
					}

					if (!isMP)
						ptsClefs[i].voisins.push_back(currentPt);
				}
			}
		}
	}

	vector<ptClef> ptsClefsInitial = ptsClefs;

	//On créé les trajectoires en partant des points clefs
	bool ended = false;
	int index = 0;
	do
	{
		vector<Point> currentTraj;
		//On initialise la trajectoire à un point clef et un de ses voisin non fait
		while (ptsClefs[index].voisins.size() == 0)
		{
			index++;
			if (index >= ptsClefs.size())
				break;
		}
		if (index < ptsClefs.size())
		{
			currentTraj.push_back(ptsClefs[index].pt);
			/*pointsDone.at<uchar>(currentTraj.back()) = 255;*/

			currentTraj.push_back(ptsClefs[index].voisins[0]);
			/*pointsDone.at<uchar>(currentTraj.back()) = 255;*/

			ptsClefs[index].voisins.erase(ptsClefs[index].voisins.begin());


			//cout << "New trajectory (no " << trajectories->size() << " )" << endl;

			bool trajEnded = false;
			do
			{

				bool found = false;
				Point ptNul(-1, -1);
				Point nextPoint = ptNul;

				//On parcours le voisinnage du dernier point de la trajectoire
				for (int xl = -1; xl <= 1; xl++)
				{
					for (int yl = -1; yl <= 1; yl++)
					{
						if (xl != yl || xl != 0)
						{

							Point lastOne(currentTraj[currentTraj.size() - 2]);
							Point evalPoint(currentTraj.back().x + yl, currentTraj.back().y + xl);
							uchar valEval = skeleton.at<uchar>(evalPoint);

							//showCurrentPoint(skeleton, evalPoint, 3);

							if (valEval && evalPoint != lastOne)
							{
								nextPoint = evalPoint;

								if (isKeyPointOrNear(nextPoint, ptsClefsInitial))
								{
									removeNearOfKeyPoint(nextPoint, &ptsClefs);

									nextPoint = ptNul;
									
								}
									
							}
						}
						if (nextPoint != ptNul)
							break;
					}
					if (nextPoint != ptNul)
						break;
				}

				if (nextPoint == ptNul )
				{
					trajEnded = true;
				}
				else
				{
					currentTraj.push_back(nextPoint);
					//showCurrentPoint(skeleton, nextPoint, 3);
				}
			} while (!trajEnded); //Tant qu'on ne retombe pas sur un point clef

			trajectories->push_back(currentTraj);
		}
		else
			ended = true;

	} while (!ended);

}

bool isKeyPointOrNear(Point pt, vector<ptClef> ptsClefs)
{
	for (int i = 0; i < ptsClefs.size(); i++)
	{
		if (ptsClefs[i].pt == pt)
			return true;
		else if (find(ptsClefs[i].voisins.begin(), ptsClefs[i].voisins.end(), pt) != ptsClefs[i].voisins.end())
			return true;
	}
	return false;
}

void removeNearOfKeyPoint(Point pt, vector<ptClef>* ptsClefs)
{
	for (int i = 0; i < ptsClefs->size(); i++)
	{
		if (find(ptsClefs->at(i).voisins.begin(), ptsClefs->at(i).voisins.end(), pt) != ptsClefs->at(i).voisins.end())
			ptsClefs->at(i).voisins.erase(remove(ptsClefs->at(i).voisins.begin(), ptsClefs->at(i).voisins.end(), pt), ptsClefs->at(i).voisins.end());

	}
}
