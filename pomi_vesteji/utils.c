#include "utils.h"
#include <math.h>

double radToDeg(double rad)
{
	return rad * 180.0 / PI;
}

double degToRad(double deg)
{
	return deg * PI / 180.0;
}

double normalizeAngle(double deg)
{
	while (deg < 0)
	{
		deg += 360;
	}

    while (deg >= 360)
	{
		deg -= 360;
	}

	return deg;
}

double distance(double xA, double yA, double xB, double yB)
{
	return sqrt((xB - xA) * (xB - xA) + (yB - yA) * (yB - yA));
}
