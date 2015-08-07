#include <stdio.h>
#include <math.h>
#include "utils.h"

int main(void)
{
	// Variabile pentru date de intrare
	int nrCopaci;
	double x1Gard, y1Gard, x2Gard, y2Gard;
	double xCopac, yCopac, lungimeCopac, unghiCadere;

	// Alte variabile auxiliare
	int i, nrTaieturi, ramaneInLivada;
	double distSus, distDreapta, distJos, distStanga, distMinima, restTaietura, aux;
	double xVarf, yVarf, xIntersectie, yIntersectie, unghiSS, unghiSD, unghiJS, unghiJD;
	
	// Citeste coordonatele gardului si numarul de copaci
	scanf("%lf%lf%lf%lf", &x1Gard, &y1Gard, &x2Gard, &y2Gard);
	scanf("%d", &nrCopaci);

	// Interschimbare puncte astfel incat A(x1, y1) sa fie punctul din stanga //
	if (x1Gard > x2Gard)
	{
		aux = x2Gard;
		x2Gard = x1Gard;
		x1Gard = aux;
	}

	if (y1Gard < y2Gard)
	{
		aux = y2Gard;
		y2Gard = y1Gard;
		y1Gard = aux;
	}

	for (i = 1; i <= nrCopaci; i++)
	{
		scanf("%lf%lf%lf%lf", &xCopac, &yCopac, &lungimeCopac, &unghiCadere);

		// Verifica daca pomul se afla in livada
		if (xCopac <= x1Gard || xCopac >= x2Gard || yCopac >= y1Gard || yCopac <= y2Gard)
		{
			printf("Pomul nu se afla in livada!\n\n");
			continue;
		}

		ramaneInLivada = 0;

		/* ----------------- Subpunct A ----------------- */
		distStanga = x2Gard - xCopac;				// Distanta dintre copac si gardul din stanga
		distSus = y1Gard - yCopac;					// Distanta dintre copac si gardul de sus
		distDreapta = xCopac - x1Gard;				// Distanta dintre copac si gardul din dreapta
		distJos = yCopac - y2Gard;					// Distanta dintre copac si gardul de jos

		// Interschimba distantele orizontale a.i. distanta minima sa fie <distStanga>
		if (distStanga > distDreapta)
		{
			aux = distDreapta;
			distDreapta = distStanga;
			distStanga = aux;
		}

		// Interschimba distantele verticale a.i. distanta minima sa fie <distSus>
		if (distSus > distJos)
		{
			aux = distSus;
			distSus = distJos;
			distJos = aux;
		}

		// Afla distanta minima
		distMinima = distStanga > distSus ? distSus : distStanga;

		// Calculeaza si afiseaza numarul de taieturi necesare si lungimea lor
		nrTaieturi = lungimeCopac / distMinima;
		restTaietura = lungimeCopac - nrTaieturi * distMinima;

		if (nrTaieturi > 0)
		{
			printf("%d %.3lf ", nrTaieturi, distMinima);
		}
		if (restTaietura > 0)
		{
			printf("1 %.3lf ", restTaietura);
		}
		/* ---------------------------------------------- */

		printf("\n");

		/* ----------------- Subpunct B ----------------- */
		// Calculeaza coordonatele punctului in care varful copacului pica
		xVarf = lungimeCopac * cos(degToRad(unghiCadere)) + xCopac;
		yVarf = lungimeCopac * sin(degToRad(unghiCadere)) + yCopac;

		// Verifica daca pomul pica in livada
		if (xVarf <= x2Gard && xVarf >= x1Gard && yVarf <= y1Gard && yVarf >= y2Gard)
		{
			printf("Pomul ramane in livada!");
			ramaneInLivada = 1;
		}
		else
		{
			// Unghiurile dreptelor ce unesc colturile gardului si copacul
			unghiSD = radToDeg(atan((y1Gard - yCopac) / (x2Gard - xCopac)));			// Unghi sus-dreapta
			unghiSS = 180 - radToDeg(atan((y1Gard - yCopac) / (xCopac - x1Gard)));		// Unghi sus-stanga
			unghiJS = 180 + radToDeg(atan((yCopac - y2Gard) / (xCopac - x1Gard)));		// Unghi jos-stanga
			unghiJD = 360 - radToDeg(atan((yCopac - y2Gard) / (x2Gard - xCopac)));		// Unghi jos-dreapta

			// Se reduce unghiul de cadere la intervalul [0, 360]
		 	unghiCadere = normalizeAngle(unghiCadere);

		 	// Calculeaza coordonatele de intersectie ale pomului cazut cu gardul in functie
		 	// de latura gardului, determinata in functie de unghiurile calculate mai sus
			if (unghiSD <= unghiCadere && unghiCadere < unghiSS)
			{
				yIntersectie = y1Gard;
				xIntersectie = (yIntersectie - yCopac) * (xVarf - xCopac)
								/ (yVarf - yCopac) + xCopac;
			}
			else if (unghiSS <= unghiCadere && unghiCadere < unghiJS)
			{
				xIntersectie = x1Gard;
				yIntersectie = (xIntersectie - xCopac) * (yVarf - yCopac)
								/ (xVarf - xCopac) + yCopac;
			}
			else if (unghiJS <= unghiCadere && unghiCadere < unghiJD)
			{
				yIntersectie = y2Gard;
				xIntersectie = (yIntersectie - yCopac) * (xVarf - xCopac)
								/ (yVarf - yCopac) + xCopac;
			}
			else
			{
				xIntersectie = x2Gard;
				yIntersectie = (xIntersectie - xCopac) * (yVarf - yCopac)
								/ (xVarf - xCopac) + yCopac;
			}

			// Afiseaza distanta de la punctul de intersectie pana la varf si coordonatele
			// punctului de intersectie
			printf(
				"%.3lf %.3lf %.3lf",
			 	distance(xIntersectie, yIntersectie, xVarf, yVarf),
			 	xIntersectie,
			 	yIntersectie
			);
		}
		/* ---------------------------------------------- */

		printf("\n");

		/* ----------------- Subpunct C ----------------- */
		// Afla distanta de la copac la punctul de intersectie
		distMinima = distance(xCopac, yCopac, xIntersectie, yIntersectie);

		if (ramaneInLivada)
		{
			printf("1 %.3lf", lungimeCopac);
		}
		else
		{
			// Calculeaza si afiseaza numarul de taieturi necesare si lungimea lor
			nrTaieturi = lungimeCopac / distMinima;
			restTaietura = lungimeCopac - nrTaieturi * distMinima;

			if (nrTaieturi > 0)
			{
				printf("%d %.3lf ", nrTaieturi, distMinima);
			}
			if (restTaietura > 0)
			{
				printf("1 %.3lf ", restTaietura);
			}
		}
		/* ---------------------------------------------- */

		printf("\n\n");
	}

	return 0;
}