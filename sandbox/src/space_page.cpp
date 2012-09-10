#include <Gosu/Audio.hpp>
#include "spherical_coordinate.h"
#include <Gosu/Inspection.hpp>
#include "PacketType.h"
#include <Gosu/Timing.hpp>
#include "config.h"
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
#include "Matrix.h"
#include <stdexcept>
#include <functional>
#include "packet.h"

SpacePage::SpacePage()
	:m_rotPlayer(Quaternion::identity())
    ,m_matGlobalToLocal(Matrix::identity())
    ,m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
{
    m_pidMine = InvalidPlayerID;
	Gosu::Input& i = PageManager::Instance()->input();
	i.setMousePosition(Gosu::screenWidth()/2, Gosu::screenHeight()/2);
	Config& config = *Config::Instance();
	m_bInvertMouse = config.get<bool>("InvertMouse", false);
	m_kbForward = config.get<Gosu::Button>("keyForward", Gosu::kbW);
	m_kbBackward = config.get<Gosu::Button>("keyBackward", Gosu::kbS);
	m_kbStrafeRight = config.get<Gosu::Button>("keyStrafeRight", Gosu::kbA);
	m_kbStrafeLeft = config.get<Gosu::Button>("keyStrafeLeft", Gosu::kbD);
	m_kbStrafeUp = config.get<Gosu::Button>("keyStrafeUp", Gosu::kbR);
	m_kbStrafeDown = config.get<Gosu::Button>("keyStrafeDown", Gosu::kbF);
	m_kbSpinLeft = config.get<Gosu::Button>("keySpinLeft", Gosu::kbQ);
	m_kbSpinRight = config.get<Gosu::Button>("keySpinRight", Gosu::kbE);
	m_kbTurbo = config.get<Gosu::Button>("keyTurbo", Gosu::kbLeftShift);
}

SpacePage::~SpacePage()
{
}

void SpacePage::render(const Renderable& r)
{
    if (m_PlayerRenderable) {
        if (r.getType() == "troll") {
            m_Closest.check(r, m_PlayerRenderable->getPosition());
        }
    }
	Gosu::Graphics& g = PageManager::Instance()->graphics();
    r.draw(m_matGlobalToLocal, g.width(), g.height());
}

void SpacePage::draw()
{
    refreshMatrix();
	Gosu::Graphics& g = PageManager::Instance()->graphics();
	double wdt = g.width();
	double hgt = g.height();
    m_Closest.draw(m_matGlobalToLocal, wdt, hgt);
    m_Closest.reset();
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
    std::wstringstream ss;
    ss << Gosu::fps();
    m_Font.drawRel(ss.str(), wdt-20, 20, 20, 1.0, 0.0);

    for (const Bullet& b:m_Bullets) {
        double len = b.dir.magnitude()*b.lifetime;
        Vector dir = b.dir.normalized();
        double step = 5.0;
        for (double d = std::max(0.0, len - b.dir.magnitude()*50.0); d < len; d += step) {
            Vector pos1 = m_matGlobalToLocal * (b.pos+Vector(0, 0, 0.1)+(dir*d));
            Vector pos2 = m_matGlobalToLocal * (b.pos+Vector(0, 0, 0.1)+(dir*(d+step)));
            SphericalCoordinate sc = pos1.toSphericalCoordinate();
            SphericalCoordinate sc_prev = pos2.toSphericalCoordinate();
            // prevent odd lines going through you from your own fire
            if (sc.distance < 10.0) continue;
            if (sc_prev.distance <= 10.0) continue;
            double x1 = Renderable::screenX(sc, wdt);
            double x2 = Renderable::screenX(sc_prev, wdt);
            // prevent stuff from spanning the screen
            if (std::abs(x1-x2) > 10) continue;
            double y1 = Renderable::screenY(sc, hgt);
            double y2 = Renderable::screenY(sc_prev, hgt);
            g.drawLine(x1, y1, Gosu::Colors::red, x2, y2, Gosu::Colors::green, 0);
        }
    }
}

void ClosestHud::check(const Renderable& r, Vector pos)
{
    double dist = (pos - r.getPosition()).magnitudeSquared();
    if (m_dDistSquared > dist || !m_bValid) {
        m_dDistSquared = dist;
        m_bValid = true;
        m_vecPos = r.getPosition();
        m_ID = r.getID();
    }
}

void ClosestHud::reset() { m_bValid = false; }

void ClosestHud::draw(const Matrix& mat, double wdt, double hgt)
{
    if (!m_bValid) return;
    Renderable rend = Renderable::temporary();
    rend.setImageName(L"sphere.png");
    rend.setScale(0.1);
    rend.setFixedSizeDistance(100);
    Gosu::Color col = Gosu::Colors::green;
    rend.setColor(col);
    rend.setPosition(m_vecPos);
    rend.draw(mat, wdt, hgt);
}

void SpacePage::rotateDegrees(Vector axis, double angle)
{
	angle = Gosu::degreesToRadians(angle);
	m_rotPlayer *= Quaternion::fromAxisAngle(axis, angle);
}

void SpacePage::update()
{
    for (size_t i = 0; i < m_Bullets.size(); i++)
    {
        Bullet& b = m_Bullets.at(i);
        b.lifetime++;
        if (b.lifetime > 200)
        {
            m_Bullets[i] = m_Bullets.back();
            m_Bullets.pop_back();
            i--;
        }
    }
    
	Gosu::Input& i = PageManager::Instance()->input();

	if (i.down(m_kbSpinLeft)) {
		rotateDegrees(Vector::FORWARD, 1);
	} else if (i.down(m_kbSpinRight)) {
		rotateDegrees(Vector::FORWARD, -1);
	}
	double wdt = PageManager::Instance()->graphics().width();
	double hgt = PageManager::Instance()->graphics().height();
	double shiftx = double(i.mouseX())-double(wdt/2);
	double shifty = double(i.mouseY())-double(hgt/2);
	double mousespeed = 0.1;
	if (shiftx != 0) {
		rotateDegrees(Vector::UP, shiftx*mousespeed);
		i.setMousePosition(wdt/2, hgt/2);
	}
	if (shifty != 0) {
		if (m_bInvertMouse) {
			rotateDegrees(Vector::RIGHT, shifty*mousespeed);
		} else {
			rotateDegrees(Vector::RIGHT, -shifty*mousespeed);
		}
		i.setMousePosition(wdt/2, hgt/2);
	}
    
    if (!m_PlayerRenderable) return;
    
	Vector dir(0, 0, 0);
	if (i.down(m_kbForward)) {
		dir += Vector::FORWARD;
	} else if (i.down(m_kbBackward)) {
		dir -= Vector::FORWARD;
	}
	if (i.down(m_kbStrafeRight)) {
		dir -= Vector::RIGHT;
	} else if (i.down(m_kbStrafeLeft)) {
		dir += Vector::RIGHT;
	}
	if (i.down(m_kbStrafeUp)) {
		dir += Vector::UP;
	} else if (i.down(m_kbStrafeDown)) {
		dir -= Vector::UP;
	}
	double speed = 1.0;
	if (!i.down(m_kbTurbo)) {
		speed = 5.0;
	}
	if (dir.x != 0 || dir.y != 0 || dir.z != 0) {
		dir.normalize();
		m_PlayerRenderable->setPosition(m_PlayerRenderable->getPosition() + m_rotPlayer * dir * speed);
        PlayerPositionChanged();
	}
}


bool SpacePage::needsCursor() const
{
	return false;
}

void SpacePage::PlayerPositionChanged()
{
    if (!m_PlayerRenderable) return;
    this->PositionChanged(*m_PlayerRenderable);
}

void SpacePage::refreshMatrix()
{
    if (!m_PlayerRenderable) {
        m_matGlobalToLocal = m_rotPlayer.inverted().toMatrix();
        return;
    }
	m_matGlobalToLocal = m_rotPlayer.inverted().toMatrix().translated(-m_PlayerRenderable->getPosition());
}

void SpacePage::buttonUp(Gosu::Button btn)
{
    if (btn == Gosu::msLeft) {
        this->firePlasma(m_rotPlayer*Vector::FORWARD);
    } else if (btn == Gosu::kbF12) {
        std::stringstream ss;
        ss << "screenshot" << time(NULL) << ".png";
        PageManager::Instance()->saveScreenShot(ss.str());
    }
}

void SpacePage::addBullet(Vector pos, Vector dir, PlayerID owner)
{
    static optional<Gosu::Sample> s_Sample;
    if (!s_Sample) {
        s_Sample.reset(Gosu::Sample(L"sfx/phaser1.wav"));
    }
    s_Sample->play();
    Bullet b;
    b.pos = pos;
    b.dir = dir.normalized()*10;
    b.lifetime = 0;
    b.owner = owner;
    m_Bullets.push_back(b);
}
