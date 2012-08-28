#ifndef SPACEPAGE_H
#define SPACEPAGE_H

#include "RenderableID.h"
#include <Gosu/Sockets.hpp>
#include <Gosu/Image.hpp>
#include "page.h" // Base class: Page
#include <Gosu/Color.hpp>
#include <Gosu/Font.hpp>
#include <iostream>
#include <cassert>
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

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


inline std::wostream& operator<<(std::wostream& o, const Vector& pos)
{
	return o << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
}

enum ZOrder
{
    zHUD = 10,
    zUI = 20
};


#include <set>
#include <memory>
struct Renderable;
struct Packet;
#include "PlayerID.h"

struct ClosestHud
{
    Vector m_vecPos;
	double m_dDistSquared;
	bool m_bValid;
    RenderableID m_ID;
    void check(const Renderable& r, Vector pos);
    void reset();
    void draw(const Matrix& mat, double wdt, double hgt);
};

class SpacePage : public Page
{
protected:
	SpacePage(const SpacePage& rhs);
	SpacePage& operator=(const SpacePage& rhs);
	Gosu::Font m_Font;
	Quaternion m_rotPlayer;
	void setupDirs();
	void rotateDegrees(Vector axis, double angle);
	Gosu::Button m_kbForward, m_kbBackward, m_kbStrafeRight, m_kbStrafeLeft;
	Gosu::Button m_kbStrafeUp, m_kbStrafeDown, m_kbSpinLeft, m_kbSpinRight;
	bool m_bInvertMouse;
	PlayerID m_pidMine;
    Matrix m_matGlobalToLocal;
    ClosestHud m_Closest;
    void refreshMatrix();
    void PlayerPositionChanged();
    Renderable* m_pPlayerRenderable;
protected:
    virtual void PositionChanged(const Renderable&) = 0;
    virtual Renderable& getEntity(RenderableID id) = 0;
    void render(const Renderable&);
public:
	void sendPacketToAll(const Packet& p);
	SpacePage();
	~SpacePage();
	virtual void update();
	bool needsCursor() const;

	virtual void draw();

};

#endif // SPACEPAGE_H
