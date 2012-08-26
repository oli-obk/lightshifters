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

#include "Quaternion.h"

#include <cmath>
#include <cstdlib>

#include "Matrix.h"
#include "Vector.h"

Quaternion::Quaternion()
{
}

Quaternion Quaternion::identity()
{
	return Quaternion::fromValues(0, 0, 0, 1);
}

Quaternion Quaternion::uninitialized()
{
	return Quaternion();
}

Quaternion Quaternion::fromValues(double x, double y, double z, double w)
{
	Quaternion quaternion;
	quaternion.x = x;
	quaternion.y = y;
	quaternion.z = z;
	quaternion.w = w;
	return quaternion;
}

Quaternion Quaternion::fromVector(Vector v)
{
	return Quaternion::fromValues(v.x, v.y, v.z, 0);
}

Vector Quaternion::toVector() const
{
	Vector vector;
	
	vector.x = x;
	vector.y = y;
	vector.z = z;
	
	return vector;
}

Quaternion Quaternion::fromAxisAngle(Vector axis, double angle)
{
	Quaternion quaternion;
	double sinAngle;
	
	angle *= 0.5;
	axis.normalize();
	sinAngle = sin(angle);
	quaternion.x = (axis.x * sinAngle);
	quaternion.y = (axis.y * sinAngle);
	quaternion.z = (axis.z * sinAngle);
	quaternion.w = cos(angle);
	
	return quaternion;
}

Matrix Quaternion::toMatrix() const
{
	double m[16];
	
	m[0]  = (1.0 - (2.0 * ((y * y) + (z * z))));
	m[1]  =         (2.0 * ((x * y) + (z * w)));
	m[2]  =         (2.0 * ((x * z) - (y * w)));
	m[3]  = 0.0;
	m[4]  =         (2.0 * ((x * y) - (z * w)));
	m[5]  = (1.0 - (2.0 * ((x * x) + (z * z))));
	m[6]  =         (2.0 * ((y * z) + (x * w)));
	m[7]  = 0.0;
	m[8]  =         (2.0 * ((x * z) + (y * w)));
	m[9]  =         (2.0 * ((y * z) - (x * w)));
	m[10] = (1.0 - (2.0 * ((x * x) + (y * y))));
	m[11] = 0.0;
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
	
	return Matrix::fromArray(m);
}

Quaternion& Quaternion::normalize()
{
	return *this = this->normalized();
}

Quaternion Quaternion::normalized() const
{
	Quaternion ret;
	double magnitude = sqrt((x * x) + (y * y) + (z * z) + (w * w));
	ret.x /= magnitude;
	ret.y /= magnitude;
	ret.z /= magnitude;
	ret.w /= magnitude;
	return ret;
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion ret;
	Vector vector1, vector2, cross;
	double angle;
	
	vector1 = toVector();
	vector2 = q.toVector();
	angle = (this->w * q.w) - vector1.dot(vector2);
	
	cross = vector1.cross(vector2);
	vector1.x *= q.w;
	vector1.y *= q.w;
	vector1.z *= q.w;
	vector2.x *= w;
	vector2.y *= w;
	vector2.z *= w;
	
	ret.x = vector1.x + vector2.x + cross.x;
	ret.y = vector1.y + vector2.y + cross.y;
	ret.z = vector1.z + vector2.z + cross.z;
	ret.w = angle;
	return ret;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	return (*this) = (*this) * q;
}

#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f

Quaternion Quaternion::slerp(const Quaternion& q, double alpha) const
{
	double startWeight, endWeight, difference;
	Quaternion result;
	
	difference = (x * q.x) + (y * q.y) + (z * q.z) + (w * q.w);
	if ((1.0 - fabs(difference)) > SLERP_TO_LERP_SWITCH_THRESHOLD) {
		double theta, oneOverSinTheta;
		
		theta = acos(fabs(difference));
		oneOverSinTheta = 1.0 / sin(theta);
		startWeight = sin(theta * (1.0 - alpha)) * oneOverSinTheta;
		endWeight = sin(theta * alpha) * oneOverSinTheta;
		if (difference < 0.0) {
			startWeight = -startWeight;
		}
	} else {
		startWeight = 1.0 - alpha;
		endWeight = alpha;
	}
	result.x = (x * startWeight) + (q.x * endWeight);
	result.y = (y * startWeight) + (q.y * endWeight);
	result.z = (z * startWeight) + (q.z * endWeight);
	result.w = (w * startWeight) + (q.w * endWeight);
	result.normalize();
	
	return result;
}

Quaternion& Quaternion::invert()
{
	return (*this) = this->inverted();
}

Quaternion Quaternion::inverted() const
{
	Quaternion q;
	double length;
	
	length = 1.0 / ((x * x) +
	                 (y * y) +
	                 (z * z) +
	                 (w * w));
	q.x = x * -length;
	q.y = y * -length;
	q.z = z * -length;
	q.w = w * length;
	return q;
}

Vector Quaternion::operator*(const Vector& vector) const
{
	Quaternion result = (*this) * (Quaternion::fromVector(vector) * inverted());
	return result.toVector();
}
