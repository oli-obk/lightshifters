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
	setposition
};

template<class T>
T& createEntity(SpacePage::EntityMap& list)
{
	std::unique_ptr<T> ptr(new T());
	T& r = *ptr;
	list.insert(std::make_pair(r.getID(), std::move(ptr)));
	return r;
}

Renderable& createEntity(SpacePage::EntityMap& list, const Packet& p)
{
	std::unique_ptr<Renderable> ptr(new Renderable(p));
	Renderable& r = *ptr;
	list.insert(std::make_pair(r.getID(), std::move(ptr)));
	return r;
}

SpacePage::SpacePage()
	:m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
	,m_rotPlayer(Quaternion::identity())
{
	m_bItPlayerEntitiesValid = false;
	m_uTrollsCaught = 0;
	try {
		m_pListenerSocket.reset(new Gosu::ListenerSocket(50042));
		m_pidNext = 1;
		m_pidMine = 0;
		m_pListenerSocket->onConnection = std::bind(&SpacePage::onConnection, this, std::placeholders::_1);
		generateSpace();
		Renderable& r = createEntity<Player>(m_mEntities);
		r.setOwner(m_pidMine);
		m_itPlayerEntity = m_mEntities.find(r.getID());
		m_bItPlayerEntitiesValid = true;
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
	Vector PlayerPosition;
	if (m_itPlayerEntity != m_mEntities.end()) {
		PlayerPosition = m_itPlayerEntity->second->getPosition();
	}
	Gosu::Graphics& g = PageManager::Instance()->graphics();
	double wdt = g.width();
	double hgt = g.height();
	Matrix mat = m_rotPlayer.inverted().toMatrix().translated(-PlayerPosition);
	Vector closestPos;
	double closestDist = 0.0;
	bool hasClosest = false;
	for (auto& it: m_mEntities) {
		const std::unique_ptr<Renderable>& obj = it.second;
		if (obj->getType() == "troll") {
			double dist = (PlayerPosition - obj->getPosition()).magnitudeSquared();
			if (closestDist > dist || !hasClosest) {
				closestDist = dist;
				hasClosest = true;
				closestPos = obj->getPosition();
			}
		}
		obj->draw(mat, wdt, hgt);
	}
	if (hasClosest) {
		Renderable rend = Renderable::temporary();
		rend.setImageName(L"sphere.png");
		rend.setScale(0.01);
		rend.setMinScale(0.1);
		Gosu::Color col = Gosu::Colors::green;
		rend.setColor(col);
		rend.setPosition(closestPos);
		rend.draw(mat, wdt, hgt);
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
	for (auto& it: m_sCommSockets) {
		(*it).update();
	}
	if (m_pListenerSocket) {
		m_pListenerSocket->update();
	}
	
	if (!m_bItPlayerEntitiesValid) {
		for(auto it = m_mEntities.begin(); it != m_mEntities.end(); it++) {
			if (it->second->getOwner() == m_pidMine) {
				m_itPlayerEntity = it;
				m_bItPlayerEntitiesValid = true;
				break;
			}
		}
		if (!m_bItPlayerEntitiesValid) return;
	}
	Renderable& playerEntity = *(m_itPlayerEntity->second);
	Gosu::Input& i = PageManager::Instance()->input();

	if (i.down(m_kbSpinLeft)) {
		rotateDegrees(Vector::FORWARD, -1);
	} else if (i.down(m_kbSpinRight)) {
		rotateDegrees(Vector::FORWARD, 1);
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
		playerEntity.setPosition(playerEntity.getPosition() + m_rotPlayer * dir * speed);
		std::cout << "changed player pos" << std::endl;
		Packet p;
		p.write(PacketType::setposition);
		p.write(playerEntity.getID());
		p.write(playerEntity.getPosition());
		sendPacketToAll(p);
	}
	std::cout << "looking for trolls" << std::endl;
	
	//double beg = Gosu::milliseconds();
	for (auto it = m_mEntities.begin(); it != m_mEntities.end();) {
		assert(it->second);
		Renderable& r = *(it->second);
		auto oldit = it++;
		// only hunt trolls
		if (r.getType() != "troll") continue;
		// and we need to catch them
		if ( (r.getPosition() - playerEntity.getPosition()).magnitudeSquared() > 10.0*10.0) continue;
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
		Troll& troll = createEntity<Troll>(m_mEntities);
		troll.setPosition(pos);
		troll.setColor(Temperature(temp).color());
	}
}

void SpacePage::onConnection(Gosu::Socket& sock)
{
	auto pair = m_sCommSockets.insert(std::unique_ptr<Gosu::CommSocket>(new Gosu::CommSocket(Gosu::cmManaged, sock)));
	Gosu::CommSocket& cs = **(pair.first);
	cs.onDisconnection = std::bind(&SpacePage::onDisconnection, this, pair.first);
	cs.onReceive = std::bind(&SpacePage::onReceive, this, m_pidNext, std::placeholders::_1, std::placeholders::_2);
	std::cout << "new connection from: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
	Renderable& r = createEntity<Player>(m_mEntities);
	r.setOwner(m_pidNext);
	Packet p;
	p.write(PacketType::addstars);
	for (auto& ptr: m_mEntities) {
		Renderable& r2 = *ptr.second;
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
		// as the server i do not accept these packets
		if (m_pListenerSocket) break;
		while (p.bytesLeftToRead()) {
			const Renderable& r = createEntity(m_mEntities, p);
			std::cout << "new item of type: " << r.getType() << " at " << r.getPosition() << " with scale " << r.getScale() << std::endl;
		}
		break;
	case PacketType::deletestars:
		// as the server i do not accept these packets
		if (m_pListenerSocket) break;
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
	case PacketType::setposition:
		while (p.bytesLeftToRead()) {
			RenderableID id = p.read<RenderableID>();
			Vector pos = p.read<Vector>();
			auto it = m_mEntities.find(id);
			if (it == m_mEntities.end()) {
				std::cout << "tried to set position of Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
				continue;
			}
			Renderable& r = *(it->second);
			// as the server i only accept these packets if the owner tries to set the position
			if (m_pListenerSocket && (r.getOwner() != player_id)) continue;
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
