#include "space_page.h"

#include <random>
#include <functional>
#include <cassert>
#include "page_manager.h"
#include <Gosu/Graphics.hpp>
#include <Gosu/Math.hpp>

size_t Star::cur_id = 0;

SpacePage::SpacePage()
{
	std::uniform_int_distribution<int64_t> posgen(-1000, 1000);
	std::uniform_int_distribution<uint32_t> temgen(0, 60000);
	std::mt19937 engine; // Mersenne twister MT19937
	for(int i = 0; i < 10000; i++) {
		Star newstar;
		newstar.pos.x = posgen(engine);
		newstar.pos.y = posgen(engine);
		newstar.pos.z = posgen(engine);
		newstar.temp.kelvin = temgen(engine);
		m_sStars.insert(newstar);
	}
	assert(m_sStars.size() == 10000);
	horizontal_angle = 0;
	vertical_angle = 0;
}

SpacePage::~SpacePage()
{
}

void SpacePage::draw()
{
	for (const Star& s:m_sStars) {
		Gosu::Graphics& g = PageManager::Instance()->graphics();
		Gosu::Color c = s.temp.color();
		int64_t x = s.pos.x - pos.x, y = s.pos.y - pos.y, z = s.pos.z - pos.z;
		double distsq = x*x+z*z;
		double dist = sqrt(distsq);
		double hor_angle = asin(z/dist);
		if (x < 0) hor_angle = M_PI/2+(M_PI/2-hor_angle);
		double ver_angle = asin(y/sqrt(distsq+y*y));
		if (distsq < 0) ver_angle = M_PI/2+(M_PI/2-ver_angle);
		x = Gosu::wrap((hor_angle-horizontal_angle)/M_PI/2*1024+1024/2, 0.0, 1024.0);
		y = Gosu::wrap((ver_angle-vertical_angle)/M_PI*768+768/2, 0.0, 768.0);
		g.drawQuad(x-1, y-1, c, x+1, y-1, c, x+1, y+1, c, x-1, y+1, c, 10);
	}
}


void SpacePage::update()
{
	Gosu::Input& i = PageManager::Instance()->input();
	if (i.down(Gosu::kbLeft)) {
		horizontal_angle -= 0.01;
	} else if(i.down(Gosu::kbRight)) {
		horizontal_angle += 0.01;
	}
	if (i.down(Gosu::kbUp)) {
		vertical_angle += 0.01;
	} else if(i.down(Gosu::kbDown)) {
		vertical_angle -= 0.01;
	}
	if (i.down(Gosu::kbV)) {
		pos.x += cos(horizontal_angle)*3;
		pos.z += sin(horizontal_angle)*3;
		pos.y += sin(vertical_angle)*3;
	} else if(i.down(Gosu::kbI)) {
		pos.x -= cos(horizontal_angle)*3;
		pos.z -= sin(horizontal_angle)*3;
		pos.y -= sin(vertical_angle)*3;
	}
}

