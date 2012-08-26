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
#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <Gosu/Image.hpp>
#include "Vector.h"
#include <map>

struct SphericalCoordinate;
struct Matrix;

class Renderable {
private:
	static std::map<std::wstring, Gosu::Image> s_Images;
	static Gosu::Image& getImage(std::wstring name);
private:
	Renderable(const Renderable& rhs) = delete;
	Renderable& operator=(const Renderable& rhs) = delete;
	Vector m_Position;
	Gosu::Image& m_rImage;
	double m_Scale;
	Gosu::Color m_Color;
protected:
	double screenX(const SphericalCoordinate& sc);
	double screenY(const SphericalCoordinate& sc);
public:
	Vector getPosition() const;
	void setPosition(Vector v);
	virtual void draw(const Matrix& mat);
	Renderable(std::wstring imagename, Vector pos, double scale = 1.0, Gosu::Color col = Gosu::Colors::white);
	virtual ~Renderable();

};

#endif // RENDERABLE_H
