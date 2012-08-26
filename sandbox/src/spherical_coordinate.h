#ifndef SPHERICALCOORDINATE_H
#define SPHERICALCOORDINATE_H

struct Vector;

class SphericalCoordinate {

public:
	double distance, azimuth, inclination;
	SphericalCoordinate(double d, double a, double i);
	Vector toCartesianCoordinate() const;
};

#endif // SPHERICALCOORDINATE_H
