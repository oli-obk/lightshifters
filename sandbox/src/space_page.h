#ifndef SPACEPAGE_H
#define SPACEPAGE_H

#include <Gosu/Sockets.hpp>
#include <Gosu/Image.hpp>
#include "page.h" // Base class: Page
#include <Gosu/Color.hpp>
#include <Gosu/Font.hpp>
#include <iostream>
#include <cassert>
#include "Vector.h"
#include "Quaternion.h"

struct dxyz {
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


inline std::wostream& operator<<(std::wostream& o, const Vector& pos)
{
	return o << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
}


#include <set>
#include <memory>
#include "renderable.h"
struct Packet;

class SpacePage : public Page
{
public:
	typedef std::map<RenderableID, std::unique_ptr<Renderable> > EntityMap;
private:
	EntityMap m_mEntities;
	EntityMap::iterator m_itPlayerEntity;
	bool m_bItPlayerEntitiesValid;
	SpacePage(const SpacePage& rhs);
	SpacePage& operator=(const SpacePage& rhs);
	Gosu::Font m_Font;
	Quaternion m_rotPlayer;
	void setupDirs();
	void rotateDegrees(Vector axis, double angle);
	Gosu::Button m_kbForward, m_kbBackward, m_kbStrafeRight, m_kbStrafeLeft;
	Gosu::Button m_kbStrafeUp, m_kbStrafeDown, m_kbSpinLeft, m_kbSpinRight;
	bool m_bInvertMouse;
	std::unique_ptr<Gosu::ListenerSocket> m_pListenerSocket;
	typedef std::set<std::unique_ptr<Gosu::CommSocket>> SocketSet;
	SocketSet m_sCommSockets;
	void generateSpace();
	void onConnection(Gosu::Socket& sock);
	void onDisconnection(SocketSet::iterator);
	void onReceive(PlayerID, const void*, std::size_t);
	PlayerID m_pidNext, m_pidMine;
	size_t m_uTrollsCaught;
public:
	void sendPacketToAll(const Packet& p);
	SpacePage();
	~SpacePage();
	virtual void update();
	bool needsCursor() const;

	virtual void draw();

};

#endif // SPACEPAGE_H
