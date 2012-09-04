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

#include "Vector.h"
#include <cmath>
#include "spherical_coordinate.h"

const Vector Vector::UP = Vector(0, 1, 0);
const Vector Vector::FORWARD = Vector(0, 0, -1);
const Vector Vector::RIGHT = Vector(1, 0, 0);

void Vector::normalize()
{
	double m = magnitude();
	x /= m;
	y /= m;
	z /= m;
}

Vector Vector::normalized() const
{
	Vector v = *this;
	v.normalize();
	return v;
}

double Vector::magnitude() const
{
	return std::sqrt(magnitudeSquared());
}

double Vector::magnitudeSquared() const
{
	return x*x + y*y + z*z;
}

Vector& Vector::operator+=(const Vector& v)
{
	return *this = (*this) + v;
}

Vector Vector::operator+(const Vector& v) const
{
	return Vector(x+v.x, y+v.y, z+v.z);
}

Vector& Vector::operator-=(const Vector& v)
{
	return *this = (*this) - v;
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(x-v.x, y-v.y, z-v.z);
}

double Vector::dot(const Vector& v) const
{
	return x*v.x+ y*v.y+ z*v.z;
}

Vector Vector::cross(const Vector& v) const
{
	Vector result;
	result.x = ((y * v.z) - (z * v.y));
	result.y = ((z * v.x) - (x * v.z));
	result.z = ((x * v.y) - (y * v.x));
	return result;
}


Vector Vector::operator*(double d) const
{
	return Vector(x * d, y * d, z * d);
}

Vector& Vector::operator*=(double d)
{
	return (*this) = (*this) * d;
}

bool Vector::operator!=(const Vector& v) const
{
	return (x != v.x || y != v.y || z != v.z);
}

bool Vector::operator==(const Vector& v) const
{
	return (x == v.x && y == v.y && z == v.z);
}

Vector Vector::operator-() const
{
	return Vector(-x, -y, -z);
}

SphericalCoordinate Vector::toSphericalCoordinate() const
{
	double mag = magnitude();
	return SphericalCoordinate(mag, atan2(-z, x), acos(y/mag));
}

bool Vector::iszero()
{
    return (x==0) && (y==0) && (z==0);
}

bool Vector::isnotzero()
{
    return (x!=0) || (y!=0) || (z!=0);
}
