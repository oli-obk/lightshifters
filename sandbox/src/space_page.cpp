#include "renderable.h"
#include "space_page.h"

#include <random>
#include <functional>
#include <cassert>
#include "page_manager.h"
#include <Gosu/Graphics.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Text.hpp>
#include <iostream>
#include <sstream>
#include "Matrix.h"

SpacePage::SpacePage()
	:m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
	,m_rotPlayer(Quaternion::identity())
{
	std::uniform_int_distribution<int64_t> posgen(-1000, 1000);
	std::uniform_int_distribution<uint32_t> tempgen(0, 60000);
	std::mt19937 engine; // Mersenne twister MT19937
	for(int i = 0; i < 100; i++) {
		Vector pos(posgen(engine), posgen(engine), posgen(engine));
		uint32_t temp = tempgen(engine);
		m_sStars.insert(std::move(std::unique_ptr<Renderable>(new Renderable(L"trollface.png", pos, 0.01, Temperature(temp).color()))));
	}
	PageManager::Instance()->input().setMousePosition(100, 100);
}

SpacePage::~SpacePage()
{
}

void SpacePage::draw()
{
	Gosu::Graphics& g = PageManager::Instance()->graphics();
	double wdt = g.width();
	double hgt = g.height();
	Matrix mat = m_rotPlayer.inverted().toMatrix().translated(-m_posPlayer);
	for (const std::unique_ptr<Renderable>& obj:m_sStars) {
		obj->draw(mat);
	}
	double xn = wdt*.25;
	double xp = wdt*.75;
	double yn = hgt*.25;
	double yp = hgt*.75;
	g.drawLine(xn, yn, Gosu::Colors::white, xp, yn, Gosu::Colors::white, 20);
	g.drawLine(xp, yn, Gosu::Colors::white, xp, yp, Gosu::Colors::white, 20);
	g.drawLine(xp, yp, Gosu::Colors::white, xn, yp, Gosu::Colors::white, 20);
	g.drawLine(xn, yp, Gosu::Colors::white, xn, yn, Gosu::Colors::white, 20);
	g.drawLine(wdt/2-20, hgt/2, Gosu::Colors::green, wdt/2+20, hgt/2, Gosu::Colors::green, 20);
	g.drawLine(wdt/2, hgt/2-20, Gosu::Colors::green, wdt/2, hgt/2+20, Gosu::Colors::green, 20);
	{
		std::wstringstream wss;
		wss << L"Pos: " << m_posPlayer;
		m_Font.draw(wss.str(), 10, 10, 10);
	}
}

void SpacePage::rotateDegrees(Vector axis, double angle)
{
	angle = Gosu::degreesToRadians(angle);
	m_rotPlayer *= Quaternion::fromAxisAngle(axis, angle);
}

void SpacePage::update()
{
	Gosu::Input& i = PageManager::Instance()->input();
	if (i.down(Gosu::kbL)) {
		rotateDegrees(Vector::FORWARD, 1);
	} else if(i.down(Gosu::kbX)) {
		rotateDegrees(Vector::FORWARD, -1);
	}
	double shiftx = i.mouseX()-100.0;
	double shifty = i.mouseY()-100.0;
	if (shiftx != 0) {
		rotateDegrees(Vector::UP, shiftx);
		i.setMousePosition(100, 100);
	}
	if (shifty != 0) {
		rotateDegrees(Vector::RIGHT, -shifty);
		i.setMousePosition(100, 100);
	}
	Vector dir(0, 0, 0);
	if (i.down(Gosu::kbV)) {
		dir += Vector::FORWARD;
	} else if(i.down(Gosu::kbI)) {
		dir -= Vector::FORWARD;
	}
	if (i.down(Gosu::kbU)) {
		dir += Vector::RIGHT;
	} else if(i.down(Gosu::kbA)) {
		dir -= Vector::RIGHT;
	}
	if (i.down(Gosu::kbC)) {
		dir += Vector::UP;
	} else if(i.down(Gosu::kbE)) {
		dir -= Vector::UP;
	}
	double speed = 1.0;
	if (i.down(Gosu::kbLeftShift)) {
		speed = 5.0;
	}
	if(dir.x != 0 || dir.y != 0 || dir.z != 0) {
		dir.normalize();
		m_posPlayer += m_rotPlayer * dir * speed;
	}
}


bool SpacePage::needsCursor() const
{
	return false;
}
