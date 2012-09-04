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
#include <Gosu/ImageData.hpp>
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
	m_FixedSizeDistance = 1000.0;
	setColor(Gosu::Colors::white);
	m_myID = s_curID;
	s_curID++;
}

Renderable::~Renderable()
{
}

double Renderable::screenX(const SphericalCoordinate& sc, double wdt)
{
	return Gosu::wrap(sc.azimuth/Gosu::pi/2*wdt + wdt*0.25, 0.0, wdt);
}

double Renderable::screenY(const SphericalCoordinate& sc, double hgt)
{
	return Gosu::wrap(sc.inclination/Gosu::pi/2*hgt*2, 0.0, hgt);
}

void Renderable::draw(const Matrix& mat, double wdt, double hgt) const
{
	SphericalCoordinate sc = (mat * m_Position).toSphericalCoordinate();
	if(sc.distance < 10.0) return; // "clipping"
	Gosu::Image& im = getImage(getImageName());
    Gosu::ImageData& imdata = im.getData();
    Quaternion quat = Quaternion::fromAxisAngle(Vector::RIGHT, -sc.inclination-Gosu::pi/2)*Quaternion::fromAxisAngle(Vector::UP, -sc.azimuth-Gosu::pi/2);
    Matrix mat2 = quat.inverted().toMatrix();
    double dist = sc.distance;
    if (dist > m_FixedSizeDistance) {
        dist = m_FixedSizeDistance;
    }
    Vector vn = mat2 * Vector(-double(im.width())/2*m_Scale, -double(im.height())/2*m_Scale, -dist);
    Vector vp = mat2 * Vector(double(im.width())/2*m_Scale, double(im.height())/2*m_Scale, -dist);
    SphericalCoordinate n = vn.toSphericalCoordinate();
    SphericalCoordinate p = vp.toSphericalCoordinate();
    auto drawfun = [m_Color, dist, &imdata](double nx, double ny, double px, double py)
        {
        imdata.draw(nx, ny, m_Color,
                    px, ny, m_Color,
                    px, py, m_Color,
                    nx, py, m_Color, -dist, Gosu::amDefault);
        };
    double nx = screenX(n, wdt);
    double px = screenX(p, wdt);
    double ny = screenY(n, hgt);
    double py = screenY(p, hgt);
    if (nx > px) {
        if (ny > py) {
            drawfun(nx - wdt, ny - hgt, px, py);
            drawfun(nx - wdt, ny, px, py + hgt);
            drawfun(nx, ny - hgt, px + wdt, py);
            drawfun(nx, ny, px + wdt, py + hgt);
        } else {
            drawfun(nx - wdt, ny, px, py);
            drawfun(nx, ny, px + wdt, py);
        }
    } else {
        if (ny > py) {
            drawfun(nx, ny - hgt, px, py);
            drawfun(nx, ny, px, py + hgt);
        } else {
            drawfun(nx, ny, px, py);
        }
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
	p.write(getFixedSizeDistance());
}

void Renderable::deserialize(const Packet& p)
{
	forceID(p.read<RenderableID>());
	setType(p.read<std::string>());
	setColor(p.read<uint32_t>());
	setPosition(p.read<Vector>());
	setScale(p.read<double>());
	setOwner(p.read<PlayerID>());
	setFixedSizeDistance(p.read<double>());
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
	m_FixedSizeDistance = other.m_FixedSizeDistance;
	other.m_myID = InvalidRenderableID;
}

Renderable::Renderable(bool)
{
	m_Scale = 1.0;
	m_FixedSizeDistance = 1000.0;
	setColor(Gosu::Colors::white);
}

PlayerID Renderable::getOwner() const
{
	return m_PlayerID;
}

void Renderable::setOwner(PlayerID id)
{
	m_PlayerID = id;
}

void Renderable::setFixedSizeDistance(double scale)
{
	m_FixedSizeDistance = scale;
}

double Renderable::getFixedSizeDistance() const
{
	return m_FixedSizeDistance;
}
