#include "stentiford.h"

using namespace cv;
using namespace std;

const auto processor_count = std::thread::hardware_concurrency();
const auto mainThreadId = std::this_thread::get_id();

/// <summary>
/// Fonction implémentant l'algorithme d'amincissement de Stentiford
/// </summary>
/// <param name="image"></param>
/// <returns></returns>
int stentiford_process(Mat image)
{
	Mat image2 = image.clone();

	const int blackRatio = 8;

	for (int i = 0; i < image2.rows; i++)
	{
		for (int j = 0; j < image2.cols; j++)
		{
			image2.at<uchar>(i, j) = 255 - image2.at<uchar>(i, j);
			image2.at<uchar>(i, j) = (image2.at<uchar>(i, j) + (blackRatio - 1) * 255) / blackRatio;
		}
	}

	int resizeRatio = 1;

	resize(image, image,
		Size(image.size().width / resizeRatio, image.size().height / resizeRatio),
		1 / resizeRatio, 1 / resizeRatio, INTER_NEAREST_EXACT);

	//Timers 
	clock_t startAll; // startIteration;
	double durationAll; // durationIteration;
	startAll = clock();

	//Binarisation de la matrice et négatif
	threshold(image, image, 100, 255, THRESH_BINARY_INV);

	//Définition des éléments
	Mat T[4];

	T[0] = Mat(3, 1, CV_8UC1, Scalar(0));
	T[1] = Mat(1, 3, CV_8UC1, Scalar(0));

	T[2] = T[0].clone();
	T[3] = T[1].clone();

	T[0].at<uchar>(0, 0) = 255;
	T[1].at<uchar>(0, 0) = 255;
	T[2].at<uchar>(2, 0) = 255;
	T[3].at<uchar>(0, 2) = 255;

	/**************************************
			Début de l'algorithme
	***************************************/
	std::cout << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "Debut de l'algorithme de stentiford." << std::endl;
	std::cout << "====================================" << std::endl;


	if (processor_count == 1)
	{
		std::cout << "Un seul core de processeur detecte... Multithreading impossible." << std::endl;

		stentiford_algorithm(image, T);
	}
	else if (processor_count == 1)
	{
		std::cout << "Erreur lors de la detection du nombre de coeurs du processeur... Multithreading impossible." << std::endl;

		stentiford_algorithm(image, T);
	}
	else
	{
		std::cout << "Nombre de coeur detectes :\t" << processor_count << std::endl;

		int nbrDecoupe;

		if (processor_count % 2 == 0)
		{
			nbrDecoupe = processor_count;
		}
		else
		{
			nbrDecoupe = processor_count - 1;
		}

		//A supprimer un jour
		//nbrDecoupe = 4;

		std::cout << "Utilisation de " << nbrDecoupe << " coeurs du processeur." << std::endl;

		std::cout << std::endl << "Decoupage de l'image..." << std::endl;

		Mat* imageCopy[2];
		imageCopy[0] = new Mat[nbrDecoupe / 2];
		imageCopy[1] = new Mat[nbrDecoupe / 2];

		for (int i = 0; i < nbrDecoupe / 2; i++)
		{
			imageCopy[0][i] = image.clone();
			imageCopy[1][i] = image.clone();
		}


		Mat* subImage[2];

		subImage[0] = new Mat[nbrDecoupe / 2];
		subImage[1] = new Mat[nbrDecoupe / 2];

		Rect roiRect1, roiRect2;

		int margeDecoupe = 3;

		for (int i = 0; i < nbrDecoupe / 2; i++)
		{
			if (i == 0)
			{
				roiRect1 = Rect(0,
					0,
					image.size().width / (nbrDecoupe / 2) + margeDecoupe,
					image.size().height / 2 + margeDecoupe);

				roiRect2 = Rect(0,
					image.size().height / 2 - margeDecoupe,
					image.size().width / (nbrDecoupe / 2) + margeDecoupe,
					image.size().height / 2 + margeDecoupe);
			}
			else if (i == nbrDecoupe / 2 - 1)
			{
				roiRect1 = Rect(i * image.size().width / (nbrDecoupe / 2) - margeDecoupe,
					0,
					image.size().width / (nbrDecoupe / 2) + margeDecoupe,
					image.size().height / 2 + margeDecoupe);

				roiRect2 = Rect(i * image.size().width / (nbrDecoupe / 2) - margeDecoupe,
					image.size().height / 2 - margeDecoupe,
					image.size().width / (nbrDecoupe / 2) + margeDecoupe,
					image.size().height / 2 + margeDecoupe);
			}
			else
			{
				roiRect1 = Rect(i * image.size().width / (nbrDecoupe / 2) - margeDecoupe,
					0,
					image.size().width / (nbrDecoupe / 2) + 2 * margeDecoupe,
					image.size().height / 2 + margeDecoupe);

				roiRect2 = Rect(i * image.size().width / (nbrDecoupe / 2) - margeDecoupe,
					image.size().height / 2 - margeDecoupe,
					image.size().width / (nbrDecoupe / 2) + 2 * margeDecoupe,
					image.size().height / 2 + margeDecoupe);
			}



			subImage[0][i] = imageCopy[0][i](roiRect1);
			rectangle(imageCopy[0][i], roiRect1, Scalar(255), 1);
			subImage[1][i] = imageCopy[1][i](roiRect2);
			rectangle(imageCopy[1][i], roiRect2, Scalar(255), 1);

			/*imshow("image 1 : " + std::to_string(i + 1), subImage[0][i]);
			imshow("image 2 : " + std::to_string(i + 1), subImage[1][i]);

			waitKey(0);*/
		}

		std::cout << std::endl << "Demarrage des threads..." << std::endl;

		std::thread* threads1;
		std::thread* threads2;

		threads1 = new std::thread[nbrDecoupe / 2];
		threads2 = new std::thread[nbrDecoupe / 2];

		//Lancement des threads
		for (int i = 0; i < nbrDecoupe / 2; i++)
		{
			threads1[i] = std::thread(stentiford_algorithm, subImage[0][i], T);
			std::cout << std::endl << "Thread numero  " << std::dec << 2 * i + 1 << "  lance. Id :\t" << std::hex << threads1[i].get_id() << std::endl;

			threads2[i] = std::thread(stentiford_algorithm, subImage[1][i], T);
			std::cout << std::endl << "Thread numero  " << std::dec << 2 * i + 2 << "  lance. Id :\t" << std::hex << threads2[i].get_id() << std::endl;
		}

		//Attente de la fin des threads
		for (int i = 0; i < nbrDecoupe / 2; i++)
		{
			threads1[i].join();

			threads2[i].join();
		}

		for (int k = 0; k < nbrDecoupe / 2; k++)
		{
			imshow("I 1 : " + std::to_string(k), subImage[0][k]);
			imshow("I 2 : " + std::to_string(k), subImage[1][k]);
		}

		bool notErased;

		//reconstitution de l'image
		for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++)
			{
				notErased = true;
				for (int k = 0; k < nbrDecoupe / 2; k++)
				{
					notErased = notErased && imageCopy[0][k].at<uchar>(i, j) && imageCopy[1][k].at<uchar>(i, j);
				}

				if (!notErased)
					image.at<uchar>(i, j) = 0;
			}
		}

		delete[] threads1;
		delete[] threads2;

		delete[] subImage[0];
		delete[] subImage[1];

		delete[] imageCopy[0];
		delete[] imageCopy[1];
	}



	durationAll = ((double)clock() - (double)startAll) / (double)CLOCKS_PER_SEC;

	std::cout << std::endl << std::endl << durationAll << " s" << std::endl << std::endl;

	resize(image, image, Size(image.size().width * resizeRatio, image.size().height * resizeRatio), resizeRatio, resizeRatio, INTER_NEAREST_EXACT);

	cvtColor(image2, image2, COLOR_GRAY2BGR);

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.at<uchar>(i, j) == 255)
				image2.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
		}
	}

	imshow("Base", image);
	imshow("Skeleton ", image2);

	waitKey(0);

	//End of function
	image.release();

	return 0;
}
/// <summary>
/// Fonction effectuant l'algorithme de stentiford
/// </summary>
/// <param name="image"></param>
/// <param name="skeleton"></param>
void stentiford_algorithm(Mat image, Mat* T)
{
	int iModifier = 0, jModifier = 1;

	Mat markedForDeletion(image.size(), CV_8UC1, Scalar(0));

	int compteur = 0;
	int compteur2 = 0;

	int i, j, i2, j2, k, correspondance, tourCorrespondance;
	int nbrConnectivities = 0;
	int nbrBlackNeighbour = 0;

	const int rows = image.rows;

	const int cols = image.cols;

	uchar value;


	do //Boucle pour les itérations
	{
		//startIteration = clock();
		//compteur++;

		markedForDeletion = Mat(image.size(), CV_8UC1, Scalar(0));

		for (k = 0; k < 4; k++) //Boucle pour les différents motifs
		{
			iModifier = !iModifier;
			jModifier = !iModifier;

			for (i = 1; i < rows - 1; i++) //Boucles pour parcourrir l'image
			{
				//std::cout << "i\t:\t" << i << std::endl;

				for (j = 1; j < cols - 1; j++)
				{
					//std::cout << std::endl;

					correspondance = 0;
					tourCorrespondance = 0;

					for (i2 = 0; i2 < 2 * iModifier + 1; i2++) //Boucles pour parcourrir l'image
					{

						for (j2 = 0; j2 < 2 * jModifier + 1; j2++)
						{
							if (correspondance == tourCorrespondance)
							{
								tourCorrespondance++;

								if (iModifier)
									value = T[k].at<uchar>(i2, 0);
								else
									value = T[k].at<uchar>(0, j2);

								/*std::cout << "           " << (int)image.at<uchar>(i + i2 - 1, j + j2 - 1);
								std::cout << "  X  " << (int)value;*/

								if (image.at<uchar>(i + i2 - 1, j + j2 - 1) == value)
									correspondance++;
							}
						}
					}

					if (correspondance == 3)
					{
						//std::cout << "Pixel trouve!" << std::endl;

						//Vérification du nombre de connectivité
						nbrConnectivities = 0;
						nbrBlackNeighbour = 0;

						if (!image.at<uchar>(i - 1, j - 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i - 1, j))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i - 1, j))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i - 1, j + 1))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i - 1, j + 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i, j + 1))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i, j + 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i + 1, j + 1))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i + 1, j + 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i + 1, j))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i + 1, j))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i + 1, j - 1))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i + 1, j - 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i, j - 1))
								nbrConnectivities++;
						}
						if (!image.at<uchar>(i, j - 1))
						{
							nbrBlackNeighbour++;
							if (image.at<uchar>(i - 1, j - 1))
								nbrConnectivities++;
						}

						if (nbrBlackNeighbour != 1 && nbrConnectivities == 1)
						{
							markedForDeletion.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}

		compteur2 = countNonZero(markedForDeletion);

		for (i = 1; i < rows - 1; i++) //Boucles pour détruire les pixels souhaités
		{
			for (j = 1; j < cols - 1; j++)
			{
				i2 = markedForDeletion.at<uchar>(i, j);
				if (i2 == 255 && image.at<uchar>(i, j) != 0)
				{
					image.at<uchar>(i, j) = 0;
				}
				else if (i2 == 255)
					compteur2--;
			}
		}


		//durationIteration = (clock() - startIteration) / (double)CLOCKS_PER_SEC;
		//durationAll = (clock() - startAll) / (double)CLOCKS_PER_SEC;
		//std::cout << "Itération no. " << compteur << " / " << compteur2 << "\t" << countNonZero(markedForDeletion) << "\t" << durationIteration << "  s.    //    " << durationAll << "  s" << std::endl;

		/*imshow("Skeleton", image);

		waitKey(0);*/
	} while (compteur2 > 0);

	if (mainThreadId != std::this_thread::get_id())
	{
		std::cout << "Thread termine. Id :\t" << std::hex << std::this_thread::get_id() << std::endl;
	}
}

