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
#include <cassert>
#include "packet.h"


std::map<std::wstring, Gosu::Image> Renderable::s_Images;
RenderableID Renderable::s_curID = 0;

Gosu::Image& Renderable::getImage(std::wstring name)
{
	auto it = s_Images.find(name);
	if (it != s_Images.end()) {
		return it->second;
	}
	auto it2 = s_Images.insert(std::pair<std::wstring, Gosu::Image>(name, Gosu::Image(PageManager::Instance()->graphics(), name)));
	return it2.first->second;
}

Renderable::Renderable()
{
	m_Scale = 1.0;
	m_MinScale = 0.0;
	setColor(Gosu::Colors::white);
	m_myID = s_curID;
	s_curID++;
}

Renderable::~Renderable()
{
}

double Renderable::screenX(const SphericalCoordinate& sc, double wdt)
{
	return Gosu::wrap(sc.azimuth/M_PI/2*wdt - wdt*0.75, 0.0, wdt);
}

double Renderable::screenY(const SphericalCoordinate& sc, double hgt)
{
	return Gosu::wrap(sc.inclination/M_PI/2*hgt*2, 0.0, hgt);
}

void Renderable::draw(const Matrix& mat, double wdt, double hgt) const
{
	SphericalCoordinate sc = (mat * m_Position).toSphericalCoordinate();
	if(sc.distance < 10.0) return; // "clipping"
	double size = std::max(1.0, 1000.0/(sc.distance+1));
	double x = screenX(sc, wdt);
	double y = screenY(sc, hgt);
	size *= m_Scale;
	if (size < m_MinScale) {
		size = m_MinScale;
	}
	Gosu::Image& im = getImage(getImageName());
	im.drawRot(x, y, -sc.distance, 0, 0.5, 0.5, size, size, m_Color);
	if (x + size*im.width()/2 > wdt) {
		im.drawRot(x - wdt, y, -sc.distance, 0, 0.5, 0.5, size, size, m_Color);
	} else if (x - size*im.width()/2 < 0) {
		im.drawRot(x + wdt, y, -sc.distance, 0, 0.5, 0.5, size, size, m_Color);
	}
}

Vector Renderable::getPosition() const
{
	return m_Position;
}

void Renderable::setPosition(Vector v)
{
	m_Position = v;
}

RenderableID Renderable::getID() const
{
	return m_myID;
}

Gosu::Color Renderable::getColor() const
{
	return m_Color;
}

void Renderable::setColor(Gosu::Color col)
{
	m_Color = col;
}

std::string Renderable::getType() const
{
	return m_Type;
}

void Renderable::forceID(RenderableID id)
{
	assert(s_curID == 0);
	m_myID = id;
}

void Renderable::setScale(double scale)
{
	m_Scale = scale;
}

double Renderable::getScale() const
{
	return m_Scale;
}

void Renderable::serialize(Packet& p) const
{
	assert(getID() != InvalidRenderableID);
	p.write(getID());
	p.write(getType());
	p.write(getColor().argb());
	p.write(getPosition());
	p.write(getScale());
	p.write(getOwner());
	p.write(getMinScale());
}

void Renderable::deserialize(const Packet& p)
{
	forceID(p.read<RenderableID>());
	setType(p.read<std::string>());
	setColor(p.read<uint32_t>());
	setPosition(p.read<Vector>());
	setScale(p.read<double>());
	setOwner(p.read<PlayerID>());
	setMinScale(p.read<double>());
}

void Renderable::setType(std::string type)
{
	m_Type = type;
	if (type == "player") {
		setImageName(L"sphere.png");
        return;
	}
    if (type == "bullet") {
        setImageName(L"sphere.png");
        return;
    }
	setImageName(L"trollface.png");
}

std::wstring Renderable::getImageName() const
{
	return m_ImageName;
}

void Renderable::setImageName(std::wstring name)
{
	m_ImageName = name;
}

Renderable::Renderable(const Packet& p)
{
	deserialize(p);
}

Renderable Renderable::temporary()
{
	return Renderable(false);
}

Renderable::Renderable(Renderable && other)
{
	m_Color = other.m_Color;
	m_ImageName = other.m_ImageName;
	m_Position = other.m_Position;
	m_Scale = other.m_Scale;
	m_Type = other.m_Type;
	m_myID = other.m_myID;
	m_PlayerID = other.m_PlayerID;
	m_MinScale = other.m_MinScale;
	other.m_myID = InvalidRenderableID;
}

Renderable::Renderable(bool)
{
	m_Scale = 1.0;
	m_Color = Gosu::Colors::white;
}

PlayerID Renderable::getOwner() const
{
	return m_PlayerID;
}

void Renderable::setOwner(PlayerID id)
{
	m_PlayerID = id;
}

void Renderable::setMinScale(double scale)
{
	m_MinScale = scale;
}

double Renderable::getMinScale() const
{
	return m_MinScale;
}
