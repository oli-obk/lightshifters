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

#ifndef MATRIX_H
#define MATRIX_H

struct Vector;

struct Matrix {
	double m[16];
	const double * operator[](unsigned row) const;
	static Matrix identity();
	static Matrix uninitialized();
	static Matrix fromArray(double arr[16]);
	static Matrix fromValues(	double m0, double m1, double m2,  double m3,
								double m4, double m5, double m6,  double m7,
								double m8, double m9, double m10, double m11,
								double m12, double m13, double m14, double m15);
	static Matrix fromCoordinateSystem(Vector right, Vector up, Vector front);
	static Matrix fromTranslation(Vector translation);
	static Matrix fromScale(Vector scale);
	Matrix operator*(const Matrix& m2) const;
	Matrix& operator*=(const Matrix& m2);
	Matrix translated(Vector v) const;
	Matrix& translate(Vector v);
	Matrix scaled(Vector v) const;
	Matrix& scale(Vector v);
	Matrix transposed() const;
	Matrix& transpose();
	Vector operator*(const Vector& v) const;
private:
	Matrix();
};

#include <iostream>
inline std::ostream& operator<<(std::ostream& o, const Matrix& m)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			o << m[i][j] << "	";
		}
		o << std::endl;
	}
	return o;
}

#endif
