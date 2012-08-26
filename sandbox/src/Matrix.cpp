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

#include "Matrix.h"

#include <cmath>
#include "Quaternion.h"
#include "Vector.h"
#include <cassert>

const double * Matrix::operator[](unsigned row) const
{
	assert(row < 4);
	return m+row*4;
}

Matrix Matrix::identity()
{
	return Matrix::fromValues(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

Matrix::Matrix() {}

Matrix Matrix::uninitialized()
{
	return Matrix();
}

Matrix Matrix::fromArray(double arr[16])
{
	Matrix m = Matrix();
	for(unsigned i = 0; i < 16; i++) m.m[i] = arr[i];
	return m;
}

Matrix Matrix::fromValues(	double m0, double m1, double m2,  double m3,
							double m4, double m5, double m6,  double m7,
							double m8, double m9, double m10, double m11,
							double m12, double m13, double m14, double m15)
{
	Matrix mat;
	mat.m[0]  = m0;
	mat.m[1]  = m1;
	mat.m[2]  = m2;
	mat.m[3]  = m3;
	mat.m[4]  = m4;
	mat.m[5]  = m5;
	mat.m[6]  = m6;
	mat.m[7]  = m7;
	mat.m[8]  = m8;
	mat.m[9]  = m9;
	mat.m[10] = m10;
	mat.m[11] = m11;
	mat.m[12] = m12;
	mat.m[13] = m13;
	mat.m[14] = m14;
	mat.m[15] = m15;
	return mat;
}

Matrix Matrix::fromCoordinateSystem(Vector right, Vector up, Vector front)
{
	return Matrix::fromValues(
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		front.x, front.y, front.z, 0,
		0, 0, 0, 1
	);
}

Matrix Matrix::operator*(const Matrix& m2) const
{
	Matrix result;
	result.m[0]  = m2.m[0] * this->m[0]  + m2.m[4] * this->m[1]  + m2.m[8]  * this->m[2]  + m2.m[12] * this->m[3];
	result.m[1]  = m2.m[1] * this->m[0]  + m2.m[5] * this->m[1]  + m2.m[9]  * this->m[2]  + m2.m[13] * this->m[3];
	result.m[2]  = m2.m[2] * this->m[0]  + m2.m[6] * this->m[1]  + m2.m[10] * this->m[2]  + m2.m[14] * this->m[3];
	result.m[3]  = m2.m[3] * this->m[0]  + m2.m[7] * this->m[1]  + m2.m[11] * this->m[2]  + m2.m[15] * this->m[3];
	result.m[4]  = m2.m[0] * this->m[4]  + m2.m[4] * this->m[5]  + m2.m[8]  * this->m[6]  + m2.m[12] * this->m[7];
	result.m[5]  = m2.m[1] * this->m[4]  + m2.m[5] * this->m[5]  + m2.m[9]  * this->m[6]  + m2.m[13] * this->m[7];
	result.m[6]  = m2.m[2] * this->m[4]  + m2.m[6] * this->m[5]  + m2.m[10] * this->m[6]  + m2.m[14] * this->m[7];
	result.m[7]  = m2.m[3] * this->m[4]  + m2.m[7] * this->m[5]  + m2.m[11] * this->m[6]  + m2.m[15] * this->m[7];
	result.m[8]  = m2.m[0] * this->m[8]  + m2.m[4] * this->m[9]  + m2.m[8]  * this->m[10] + m2.m[12] * this->m[11];
	result.m[9]  = m2.m[1] * this->m[8]  + m2.m[5] * this->m[9]  + m2.m[9]  * this->m[10] + m2.m[13] * this->m[11];
	result.m[10] = m2.m[2] * this->m[8]  + m2.m[6] * this->m[9]  + m2.m[10] * this->m[10] + m2.m[14] * this->m[11];
	result.m[11] = m2.m[3] * this->m[8]  + m2.m[7] * this->m[9]  + m2.m[11] * this->m[10] + m2.m[15] * this->m[11];
	result.m[12] = m2.m[0] * this->m[12] + m2.m[4] * this->m[13] + m2.m[8]  * this->m[14] + m2.m[12] * this->m[15];
	result.m[13] = m2.m[1] * this->m[12] + m2.m[5] * this->m[13] + m2.m[9]  * this->m[14] + m2.m[13] * this->m[15];
	result.m[14] = m2.m[2] * this->m[12] + m2.m[6] * this->m[13] + m2.m[10] * this->m[14] + m2.m[14] * this->m[15];
	result.m[15] = m2.m[3] * this->m[12] + m2.m[7] * this->m[13] + m2.m[11] * this->m[14] + m2.m[15] * this->m[15];
	return result;
}

Matrix& Matrix::operator*=(const Matrix& m2)
{
	return (*this) = (*this) * m2;
}

Matrix Matrix::fromTranslation(Vector t)
{
	return Matrix::fromValues(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		t.x, t.y, t.z, 1
	);
}

Matrix& Matrix::translate(Vector v)
{
	Vector t = (*this)*v;
	m[3] = t.x;
	m[7] = t.y;
	m[11] = t.z;
	return (*this);
}

Matrix Matrix::translated(Vector v) const
{
	return Matrix(*this).translate(v);
}

Matrix Matrix::fromScale(Vector scale)
{
	return Matrix::fromValues(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	);
}

Matrix& Matrix::scale(Vector v)
{
	Matrix scalingMatrix = Matrix::fromScale(v);
	return (*this) *= scalingMatrix;
}

Matrix Matrix::scaled(Vector v) const
{
	Matrix scalingMatrix = Matrix::fromScale(v);
	return (*this) * scalingMatrix;
}

Matrix& Matrix::transpose()
{
	return *this = transposed();
}

Matrix Matrix::transposed() const
{
	return Matrix::fromValues(	m[0], m[4], m[8], m[12],
								m[1], m[5], m[9], m[13],
								m[2], m[6], m[10],m[14],
								m[3], m[7], m[11],m[15]);
}

Vector Matrix::operator*(const Vector& v) const
{
	Vector result;
	result.x = ((m[0] * v.x) + (m[1] * v.y) + (m[2]  * v.z) + m[3]);
	result.y = ((m[4] * v.x) + (m[5] * v.y) + (m[6]  * v.z) + m[7]);
	result.z = ((m[8] * v.x) + (m[9] * v.y) + (m[10] * v.z) + m[11]);
	return result;
}
