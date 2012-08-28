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

struct Temperature {
	Temperature(uint32_t k):kelvin(k) {}
	uint32_t kelvin;
	Gosu::Color color() const {
		if (kelvin <= 300) {
			return Gosu::Color(255*300/2600, 0, 0);
		} else if (kelvin <= 2600) {
			return Gosu::Color(255*kelvin/2600, 0, 0);
		} else if (kelvin <= 6000) {
			return Gosu::Color(255, 255*(kelvin-2600)/(6000-2600), 0);
		} else if (kelvin <= 10000) {
			return Gosu::Color(255, 255, 255*(kelvin-6000)/(10000-6000));
		} else if (kelvin <= 33000) {
			int val = 255-(255-100)*(kelvin-10000)/(33000-10000);
			return Gosu::Color(val, val, 255);
		} else {
			return Gosu::Color(100, 100, 255);
		}
	}
};

struct SphericalCoordinate;
struct Matrix;

#include "PlayerID.h"
#include "RenderableID.h"
struct Packet;

class Renderable
{
private:
	static std::map<std::wstring, Gosu::Image> s_Images;
	static RenderableID s_curID;
protected:
	static Gosu::Image& getImage(std::wstring name);
private:
	RenderableID m_myID;
	Renderable(const Renderable& rhs) = delete;
	Renderable& operator=(const Renderable& rhs) = delete;
	Vector m_Position;
	double m_Scale;
	Gosu::Color m_Color;
	std::string m_Type;
	std::wstring m_ImageName;
	PlayerID m_PlayerID;
	void forceID(RenderableID id);
	void deserialize(const Packet& p);
	double m_MinScale;
protected:
	Renderable();
	explicit Renderable(bool);
public:
	void setMinScale(double);
	double getMinScale() const;
	PlayerID getOwner() const;
	void setOwner(PlayerID id);
	Renderable(Renderable &&);
	static Renderable temporary();
	void serialize(Packet& p) const;
	void setScale(double scale);
	double getScale() const;
	void setColor(Gosu::Color col);
	std::string getType() const;
	std::wstring getImageName() const;
	void setType(std::string type);
	RenderableID getID() const;
	Vector getPosition() const;
	Gosu::Color getColor() const;
	void setPosition(Vector v);
	virtual void draw(const Matrix& mat, double wdt, double hgt) const;
	Renderable(const Packet& p);
	virtual ~Renderable();
	void setImageName(std::wstring name);
    
    static double screenX(const SphericalCoordinate& sc, double wdt);
    static double screenY(const SphericalCoordinate& sc, double hgt);
};

#endif // RENDERABLE_H
