#include "utilitaire.h"

using namespace std;
using namespace cv;

QString endlQt = "\n";

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

    qInfo() << endlQt << QString::fromStdString(affichage) << endlQt;
}

void printVoidSeparator(char character)
{
	string affichage;

	for (int i = 0; i < SEPARATOR_LENGTH; i++)
	{
		affichage += character;
	}

    qInfo() << endlQt << endlQt << QString::fromStdString(affichage) << endlQt;
}


void showCurrentPoint(Mat skeleton, Point pt, uint scale)
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

	if (pt != Point(0, 0))
		circle(toprint, pt, 5, Vec3b(0, 0, 255));

	resize(toprint, toprint, Size(toprint.size().width * scale, toprint.size().height * scale), scale, scale, INTER_NEAREST_EXACT);
	imshow("Current Point", toprint);
	waitKey(0);
}

void drawTrajectories(cv::Mat skeleton, std::vector<std::vector<cv::Point>> trajectories, uint pointPerFrame, uint scale)
{
	//Matrice pour l'affichage progressif des données;
	Mat drawing = Mat(skeleton.size(), CV_8UC3, Scalar(0));

	//initialisation pour l'utilisation de rand()
	srand(time(NULL));

    for (uint i = 0; i < trajectories.size(); i++)
	{
		Vec3b couleur(rand() % 205 + 50, rand() % 205 + 50, rand() % 205 + 50);

        for (uint j = 0; j < trajectories[i].size(); j++)
		{

			drawing.at<Vec3b>(trajectories[i][j]) = couleur;
			if (j % pointPerFrame == 0)
			{
				Mat temp = drawing;
				resize(temp, temp, Size(temp.size().width * scale, temp.size().height * scale), scale, scale, INTER_NEAREST_EXACT);
				imshow("Trajectoires", temp);
				waitKey(0);
			}
		}
	}

}

void fusionTrajectories(vector<vector<Point>> trajIn, vector<vector<Point>>* trajOut, uint distanceMax)
{
	//Trajectoires de sorties
	vector<vector<Point>> out;
	int index = 0;

	//Tableau de bool pour savoir quelles trajectoires on deja été ajoutée à out
	int nbrTraj = trajIn.size();
	bool* trajDone = new bool[nbrTraj];
	bool allTrajDone = false;

	//On initialise le tableau 
	for (int i = 0; i < nbrTraj; i++)
	{
		trajDone[i] = false;
	}

	//On ajoute la premiere trajectoire à a sortie
	out.push_back(trajIn[0]);
	trajDone[0] = true;

	Point lastPoint = out.back().back();

	//Tant qu'on a pas fait toutes les trajectoires
	do
	{
		uint distanceMin2 = -1; // Over nine thousand
		int closerIndex = -1;
		bool isReversed = false;
		uint oldIndex = index;

		lastPoint = out.back().back(); 
		//On calcul la distance avec le point le plus proche parmis les extrémités des autres trajectoires non faites
		for (int i = 0; i < nbrTraj; i++)
		{
			if (!trajDone[i]) // On ne prend que les trajectoire pas deja ajoutee à la sortie
			{
				uint dist = pow(lastPoint.x - trajIn[i][0].x, 2) + pow(lastPoint.y - trajIn[i][0].y, 2);
				//On calcul la distance par rapport au premier point
				if (distanceMin2 > dist)
				{
					distanceMin2 = dist;
					closerIndex = i; 
					isReversed = false;
				}
				dist = pow(lastPoint.x - trajIn[i].back().x, 2) + pow(lastPoint.y - trajIn[i].back().y, 2);
				//On calcul la distance par rapport au dernier point
				if (distanceMin2 > dist)
				{
					distanceMin2 = dist;
					closerIndex = i;
					isReversed = true;
				}
			}
		}

		//Si la distance est suffisemment petite
		if (sqrt(distanceMin2) < distanceMax)
		{
			
		}
		else
		{
			index++;
			
		}

		vector<Point> temp = trajIn[closerIndex];
		trajDone[closerIndex] = true;

		if (isReversed)
			reverse(temp.begin(), temp.end());

		if (index == oldIndex)
			out[index].insert(out[index].end(), temp.begin(), temp.end());
		else
			out.push_back(temp);

		//On vérifie si on a ajouté toutes les trajectoires
		allTrajDone = true;
		for (int i = 0; i < nbrTraj; i++)
		{
			allTrajDone = allTrajDone && trajDone[i];
		}
	} while (!allTrajDone);

	//On desalloue le tableau de bool
	delete[] trajDone;

	//On copie la sortie dans la variable passée en paramètre
	*trajOut = out;
}

void lightenTrajectories(vector<vector<Point>> trajIn, vector<vector<Point>>* trajOut, uint spacing)
{
	//Trajectoires de sorties
	vector<vector<Point>> out = trajIn;
	int nbrTraj = trajIn.size();

	for (int i = 0; i < nbrTraj; i++)
	{
		int index = 0;
		while (index + spacing < out[i].size())
		{
            for (uint j = 0; j < spacing; j++)
			{
				out[i].erase(out[i].begin() + index + 1);
			}
			index++;
		}
        for (uint j = index; j < out[i].size() - 1; j++)
		{
			out[i].erase(out[i].begin() + index + 1);
		}
	}
	

	//On assigne out à la variable par référence
	*trajOut = out;
}
