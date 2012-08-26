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

#include <Gosu/Math.hpp>
#include "renderable.h"
#include "Vector.h"
#include "Matrix.h"
#include "spherical_coordinate.h"
#include <cmath>
#include "page_manager.h"
#include <Gosu/Graphics.hpp>


std::map<std::wstring, Gosu::Image> Renderable::s_Images;

Gosu::Image& Renderable::getImage(std::wstring name)
{
	auto it = s_Images.find(name);
	if (it != s_Images.end()) {
		return it->second;
	}
	auto it2 = s_Images.insert(std::pair<std::wstring, Gosu::Image>(name, Gosu::Image(PageManager::Instance()->graphics(), name)));
	return it2.first->second;
}

Renderable::Renderable(std::wstring imagename, Vector pos, double scale, Gosu::Color col)
:m_Position(pos)
,m_rImage(getImage(imagename))
,m_Scale(scale)
,m_Color(col)
{
}

Renderable::~Renderable()
{
}

double Renderable::screenX(const SphericalCoordinate& sc)
{
	double wdt = PageManager::Instance()->graphics().width();
	return Gosu::wrap(sc.azimuth/M_PI/2*wdt - wdt*0.75, 0.0, wdt);
}

double Renderable::screenY(const SphericalCoordinate& sc)
{
	double hgt = PageManager::Instance()->graphics().height();
	return Gosu::wrap(sc.inclination/M_PI/2*hgt*2, 0.0, hgt);
}

void Renderable::draw(const Matrix& mat)
{
	SphericalCoordinate sc = (mat * m_Position).toSphericalCoordinate();
	double size = std::max(1.0, 1000.0/(sc.distance+1));
	m_rImage.drawRot(screenX(sc), screenY(sc), -sc.distance, 0, 0.5, 0.5, size*m_Scale, size*m_Scale, m_Color);
}

Vector Renderable::getPosition() const
{
	return m_Position;
}

void Renderable::setPosition(Vector v)
{
	m_Position = v;
}

