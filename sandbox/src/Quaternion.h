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

#ifndef QUATERNION_H
#define QUATERNION_H

struct Vector;
struct Matrix;

struct Quaternion {
	double x, y, z, w;
	static Quaternion identity();
	static Quaternion uninitialized();
	static Quaternion fromValues(double x, double y, double z, double w);
	static Quaternion fromVector(Vector v);
	static Quaternion fromAxisAngle(Vector axis, double angle);
	Vector toVector() const;
	Matrix toMatrix() const;
	Quaternion& normalize();
	Quaternion normalized() const;
	Quaternion operator*(const Quaternion& q) const;
	Quaternion& operator*=(const Quaternion& q);
	Quaternion slerp(const Quaternion& q, double alpha) const;
	Quaternion& invert();
	Quaternion inverted() const;
	Vector operator*(const Vector& v) const;
private:
	Quaternion();
};

#endif
