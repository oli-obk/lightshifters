/*
  Copyright (C) 2012 Oliver Schneider

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Oliver Schneider <mail@oli-obk.de>
*/

#ifndef VECTOR_H
#define VECTOR_H

struct SphericalCoordinate;

struct Vector {
	static const Vector UP, FORWARD, RIGHT;
	typedef double type;
	double x;
	double y;
	double z;
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
		#error you can use constructor delegation now
	#endif
	Vector():x(0),y(0),z(0) {}
	Vector(double x, double y, double z):x(x),y(y),z(z) {}
	SphericalCoordinate toSphericalCoordinate() const;
	void normalize();
	Vector normalized() const;
	double magnitude() const;
	double magnitudeSquared() const;
	Vector operator+(const Vector& v) const;
	Vector& operator+=(const Vector& v);
	Vector operator-(const Vector& v) const;
	Vector& operator-=(const Vector& v);
	double dot(const Vector& v) const;
	Vector cross(const Vector& v) const;
	Vector operator*(double d) const;
	Vector& operator*=(double d);
	bool operator!=(const Vector& v) const;
	bool operator==(const Vector& v) const;
	Vector operator-() const;
};

#include <iostream>
inline std::ostream& operator<<(std::ostream& o, const Vector& v)
{
	return o << v.x << "," << v.y << "," << v.z;
}

#endif // VECTOR_H
