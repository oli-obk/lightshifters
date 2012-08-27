#include "player.h"
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
#include <sstream>
#include "Matrix.h"
#include <stdexcept>
#include <functional>
#include "packet.h"
#include "troll.h"

enum class PacketType : uint8_t
{
    addstars,
    deletestars,
	setposition,
	setplayerposition
};

SpacePage::SpacePage()
	:m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
	,m_rotPlayer(Quaternion::identity())
{
	m_uTrollsCaught = 0;
	try {
		m_pListenerSocket.reset(new Gosu::ListenerSocket(50042));
		m_pidNext = 1;
		m_pidMine = 0;
		m_pListenerSocket->onConnection = std::bind(&SpacePage::onConnection, this, std::placeholders::_1);
		generateSpace();
		std::unique_ptr<Renderable> ptr(new Player());
		Renderable& r = *ptr;
		m_mEntities.insert(std::make_pair(r.getID(), std::move(ptr)));
		m_mPlayerIDToRenderable[m_pidMine] = r.getID();
	} catch (const std::runtime_error& e) {
		if (e.what() != std::string("Address already in use")) {
			throw e;
		}
		m_pidNext = 0;
		m_pidMine = -1;
		auto pair = m_sCommSockets.insert(std::unique_ptr<Gosu::CommSocket>(new Gosu::CommSocket(Gosu::cmManaged, Gosu::stringToAddress("localhost"), 50042)));
		Gosu::CommSocket& cs = **(pair.first);
		cs.onDisconnection = std::bind(&SpacePage::onDisconnection, this, pair.first);
		cs.onReceive = std::bind(&SpacePage::onReceive, this, 0, std::placeholders::_1, std::placeholders::_2);
	}
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
	for (auto& it: m_mEntities) {
		const std::unique_ptr<Renderable>& obj = it.second;
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
		wss << L"Trolls caught: " << m_uTrollsCaught;
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

	if (i.down(m_kbSpinLeft)) {
		rotateDegrees(Vector::FORWARD, -1);
	} else if (i.down(m_kbSpinRight)) {
		rotateDegrees(Vector::FORWARD, 1);
	}
	double shiftx = double(i.mouseX())-double(Gosu::screenWidth()/2);
	double shifty = double(i.mouseY())-double(Gosu::screenHeight()/2);
	double mousespeed = 0.1;
	if (shiftx != 0) {
		rotateDegrees(Vector::UP, shiftx*mousespeed);
		i.setMousePosition(Gosu::screenWidth()/2, Gosu::screenHeight()/2);
	}
	if (shifty != 0) {
		if (m_bInvertMouse) {
			rotateDegrees(Vector::RIGHT, shifty*mousespeed);
		} else {
			rotateDegrees(Vector::RIGHT, -shifty*mousespeed);
		}
		i.setMousePosition(Gosu::screenWidth()/2, Gosu::screenHeight()/2);
	}
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
	if (i.down(Gosu::kbLeftShift)) {
		speed = 5.0;
	}
	if (dir.x != 0 || dir.y != 0 || dir.z != 0) {
		dir.normalize();
		m_posPlayer += m_rotPlayer * dir * speed;
		if (m_pListenerSocket) {
			auto id = m_mPlayerIDToRenderable[m_pidMine];
			Packet p;
			p.write(PacketType::setposition);
			p.write(id);
			p.write(m_posPlayer);
			sendPacketToAll(p);
		} else {
			Packet p;
			p.write(PacketType::setplayerposition);
			p.write(m_posPlayer);
			sendPacketToAll(p);
		}
	}
	
	//double beg = Gosu::milliseconds();
	for (auto it = m_mEntities.begin(); it != m_mEntities.end();) {
		Renderable& r = *(it->second);
		auto oldit = it++;
		// only hunt trolls
		if (r.getType() != "troll") continue;
		// and we need to catch them
		if ( (r.getPosition() - m_posPlayer).magnitudeSquared() > 10.0*10.0) continue;
		// tell everyone i got it
		RenderableID id = r.getID();
		Packet p;
		p.write(PacketType::deletestars);
		p.write(id);
		sendPacketToAll(p);
		m_mEntities.erase(oldit);
		m_uTrollsCaught++;
	}
	//double ms = Gosu::milliseconds()-beg;
	//std::cout << ms << std::endl;
	for (auto& it: m_sCommSockets) {
		(*it).update();
	}
	if (m_pListenerSocket) {
		m_pListenerSocket->update();
	}
}


bool SpacePage::needsCursor() const
{
	return false;
}

void SpacePage::generateSpace()
{
	std::uniform_real_distribution<double> posgen(-1000, 1000);
	std::uniform_int_distribution<uint32_t> tempgen(0, 60000);
	std::mt19937 engine; // Mersenne twister MT19937
	for (int i = 0; i < 100; i++) {
		Vector pos(posgen(engine), posgen(engine), posgen(engine));
		uint32_t temp = tempgen(engine);
		std::unique_ptr<Troll> ptr(new Troll());
		ptr->setPosition(pos);
		ptr->setColor(Temperature(temp).color());
		auto id = ptr->getID();
		m_mEntities.insert(std::make_pair(id, std::move(ptr)));
	}
}

void SpacePage::onConnection(Gosu::Socket& sock)
{
	auto pair = m_sCommSockets.insert(std::unique_ptr<Gosu::CommSocket>(new Gosu::CommSocket(Gosu::cmManaged, sock)));
	Gosu::CommSocket& cs = **(pair.first);
	cs.onDisconnection = std::bind(&SpacePage::onDisconnection, this, pair.first);
	cs.onReceive = std::bind(&SpacePage::onReceive, this, m_pidNext, std::placeholders::_1, std::placeholders::_2);
	std::cout << "new connection from: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
	std::unique_ptr<Renderable> ptr(new Player());
	const Renderable& r = *ptr;
	m_mEntities.insert(std::make_pair(r.getID(), std::move(ptr)));
	{
		Packet p;
		p.write(PacketType::addstars);
		r.serialize(p);
		sendPacketToAll(p);
	}
	m_mPlayerIDToRenderable[m_pidNext] = r.getID();
	Packet p;
	p.write(PacketType::addstars);
	for (auto& ptr: m_mEntities) {
		Renderable& r2 = *ptr.second;
		if (r2.getID() == r.getID()) continue;
		r2.serialize(p);
	}
	p.writeTo(cs);
	cs.sendPendingData();
	m_pidNext++;
}

void SpacePage::onDisconnection(SocketSet::iterator it)
{
	// the Socket is already invalid, DO NOT ACCESS INSIDE THIS FUNCTION
	//const Gosu::CommSocket& cs = **it;
	//std::cout << "lost connection to: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
	m_sCommSockets.erase(it);
}

void SpacePage::onReceive(PlayerID player_id, const void* data, std::size_t size)
{
	Packet p(data, size);
	p.beginRead();
	PacketType pt = p.read<PacketType>();
	switch (pt) {
	case PacketType::addstars:
		while (p.bytesLeftToRead()) {
			std::unique_ptr<Renderable> ptr(new Renderable(p));
			std::cout << "new item of type: " << ptr->getType() << " at " << ptr->getPosition() << " with scale " << ptr->getScale() << std::endl;
			auto id = ptr->getID();
			m_mEntities.insert(std::make_pair(id, std::move(ptr)));
		}
		break;
	case PacketType::deletestars:
		while (p.bytesLeftToRead()) {
			RenderableID id = p.read<RenderableID>();
			auto it = m_mEntities.find(id);
			if (it == m_mEntities.end()) {
				std::cout << "tried to erase Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
				continue;
			}
			m_mEntities.erase(it);
		}
		break;
	case PacketType::setplayerposition:
		while (p.bytesLeftToRead()) {
			RenderableID id = m_mPlayerIDToRenderable[player_id];
			Vector pos = p.read<Vector>();
			std::cout << "setplayerposition: " << pos << std::endl;
			auto it = m_mEntities.find(id);
			if (it == m_mEntities.end()) {
				std::cout << "tried to set position of Player " << player_id << ", but no renderable was found" << std::endl;
				continue;
			}
			(it->second)->setPosition(pos);
			Packet p;
			p.write(PacketType::setposition);
			p.write(id);
			p.write(pos);
			sendPacketToAll(p);
		}
	break;
	case PacketType::setposition:
		while (p.bytesLeftToRead()) {
			RenderableID id = p.read<RenderableID>();
			Vector pos = p.read<Vector>();
			std::cout << "setposition: " << pos << std::endl;
			auto it = m_mEntities.find(id);
			if (it == m_mEntities.end()) {
				std::cout << "tried to set position of Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
				continue;
			}
			(it->second)->setPosition(pos);
		}
	break;
	default:
		std::cout << "invalid packet type received: " << static_cast<uint32_t>(pt) << std::endl;
		break;
	}
	p.endRead();
}

void SpacePage::sendPacketToAll(const Packet& p)
{
	for (auto& ptr:m_sCommSockets) {
		Gosu::CommSocket& cs = *ptr;
		p.writeTo(cs);
		cs.sendPendingData();
	}
}
