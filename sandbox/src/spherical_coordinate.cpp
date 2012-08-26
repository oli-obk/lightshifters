#include "spherical_coordinate.h"
#include "Vector.h"
#include <cmath>

SphericalCoordinate::SphericalCoordinate(double d, double a, double i)
:distance(d), azimuth(a), inclination(i)
{
}


Vector SphericalCoordinate::toCartesianCoordinate() const
{
	double sinInc = std::sin(inclination);
	return Vector(	distance * sinInc * std::cos(azimuth),
					distance * sinInc * std::sin(azimuth),
					distance * std::cos(inclination)
				);
}