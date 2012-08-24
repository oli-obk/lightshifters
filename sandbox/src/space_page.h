#ifndef SPACEPAGE_H
#define SPACEPAGE_H

#include "page.h" // Base class: Page
#include <Gosu/Color.hpp>

struct dxyz
{
	dxyz():x(0),y(0),z(0) {}
	double x, y, z;
};

static const double s_to_min = 1.0/60.0;
static const double min_to_h = 1.0/60.0;
static const double s_to_h = 1.0/3600.0;
static const double h_to_d = 1.0/24.0;
static const double min_to_d = 1.0/1440.0;
static const double s_to_d = 1.0/86400.0;
static const double d_to_y = 1.0/365.25;
static const double s_to_y = 1.0/31557600.0;
static const double min_to_y = 1.0/525960.0;
static const double h_to_y = 1.0/8766.0;

struct Position
{
	int64_t x, y, z; // in ls
	Position():x(0),y(0),z(0) {}
};

struct Temperature
{
	uint32_t kelvin;
	Gosu::Color color() const
	{
		if (kelvin <= 2600) {
			return Gosu::Color(255*2600/kelvin, 0, 0);
		} else if(kelvin <= 6000) {
			return Gosu::Color(255, 255*(6000-2600)/(kelvin-2600), 0);
		} else if(kelvin <= 10000) {
			return Gosu::Color(255, 255, 255*(10000-6000)/(kelvin-6000));
		} else if(kelvin <= 33000) {
			int val = 255-(255-100)*(33000-10000)/(kelvin-10000);
			return Gosu::Color(val, val, 255);
		} else {
			return Gosu::Color(100, 100, 255);
		}
	}
};

struct Star
{
private:
	static size_t cur_id;
public:
	Star()
	{
		id = cur_id;
		cur_id++;
	}
	size_t id;
	Position pos;
	Temperature temp;
	bool operator<(const Star& rhs) const { return id < rhs.id; }
};

#include <set>

class SpacePage : public Page {

private:
	std::set<Star> m_sStars;
	Position pos;
	SpacePage(const SpacePage& rhs);
	SpacePage& operator=(const SpacePage& rhs);
	
	double horizontal_angle, vertical_angle;

public:
	SpacePage();
	~SpacePage();
	virtual void update();
	virtual void draw();

};

#endif // SPACEPAGE_H
