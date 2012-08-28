#include "PacketType.h"
#include "server_page.h"
#include "player.h"
#include "packet.h"
#include "troll.h"
#include "RenderableID.h"
#include <sstream>

template<class T>
T& createEntity(ServerPage::EntityMap& list)
{
    std::unique_ptr<T> ptr(new T());
    T& r = *ptr;
    list.insert(std::make_pair(r.getID(), std::move(ptr)));
    return r;
}

ServerPage::ServerPage(uint16_t port)
    :m_ListenerSocket(port)
{
    m_pidNext = 1;
    m_pidMine = 0;
    m_ListenerSocket.onConnection = std::bind(&ServerPage::onConnection, this, std::placeholders::_1);
    generateSpace();
    Renderable& r = createEntity<Player>(m_mEntities);
    m_mPlayers[m_pidMine].PlayerEntity = r.getID();
    r.setOwner(m_pidMine);
    m_pPlayerRenderable = &r;
}

void ServerPage::onConnection(Gosu::Socket& sock)
{
    auto pair = m_sClients.insert(std::make_pair(m_pidNext, new Gosu::CommSocket(Gosu::cmManaged, sock)));
    Gosu::CommSocket& cs = *(pair.first->second);
    cs.onDisconnection = std::bind(&ServerPage::onDisconnection, this, pair.first);
    cs.onReceive = std::bind(&ServerPage::onReceive, this, m_pidNext, std::placeholders::_1, std::placeholders::_2);
    std::cout << "new connection from: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
    Renderable& r = createEntity<Player>(m_mEntities);
    m_mPlayers[m_pidNext].PlayerEntity = r.getID();
    r.setOwner(m_pidNext);
    Packet p2;
    p2.write(PacketType::set_player_id);
    p2.write(m_pidNext);
    p2.writeTo(cs);
    Packet p;
    p.write(PacketType::create_entities);
    for (auto& ptr: m_mEntities) {
        Renderable& r2 = *ptr.second;
        r2.serialize(p);
    }
    p.writeTo(cs);
    Packet p3;
    p3.write(PacketType::scoreboard);
    for (auto& ptr: m_mPlayers) {
        p3.write(ptr.first);
        p3.write<uint32_t>(ptr.second.TrollsCaught);
    }
    p3.writeTo(cs);
    cs.sendPendingData();
    m_pidNext++;
}

void ServerPage::generateSpace()
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

ServerPage::~ServerPage()
{
}

void ServerPage::update()
{
    SpacePage::update();
    m_ListenerSocket.update();
    for(auto& it : m_sClients) {
        it.second->update();
    }
    if (!m_pPlayerRenderable) return;
    if (m_Closest.m_bValid && m_Closest.m_dDistSquared < 10*10) {
        eraseEntity(m_Closest.m_ID);
        m_mPlayers[m_pidMine].TrollsCaught++;
    }
}

void ServerPage::draw()
{
    SpacePage::draw();
    for (auto& it: m_mEntities) {
        const Renderable& r = *it.second;
        render(r);
    }
    {
        double pos = 10;
            for (auto& it: m_mPlayers) {
            std::wstringstream wss;
            if (it.first == m_pidMine) {
                wss << L"You";
            } else {
                wss << L"Player " << it.first;
            }
            wss << L" caught " << it.second.TrollsCaught << L" Troll";
            if (it.second.TrollsCaught != 1) {
                wss << L"s";
            }
            m_Font.draw(wss.str(), 10, pos, 10);
            pos += 15;
        }
	}
}



void ServerPage::onDisconnection(SocketSet::iterator it)
{
    // the Socket is already invalid, DO NOT ACCESS INSIDE THIS FUNCTION
    //const Gosu::CommSocket& cs = **it;
    //std::cout << "lost connection to: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
    m_sClients.erase(it);
}


void ServerPage::onReceive(PlayerID player_id, const void* data, std::size_t size)
{
    Packet p(data, size);
    p.beginRead();
    PacketType pt = p.read<PacketType>();
    switch (pt) {
    case PacketType::create_entities:
        // as the server i do not accept these packets
        std::cout << "client tried to create/change entities" << std::endl;
        break;
    case PacketType::delete_entities:
        // as the server i do not accept these packets
        std::cout << "client tried to delete entities" << std::endl;
        break;
    case PacketType::set_entity_position:
        while (p.bytesLeftToRead()) {
            RenderableID id = p.read<RenderableID>();
            Vector pos = p.read<Vector>();
            auto it = m_mEntities.find(id);
            if (it == m_mEntities.end()) {
                std::cout << "tried to set position of Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
                continue;
            }
            Renderable& r = *(it->second);
            // i only accept these packets if they are from the owner
            if (r.getOwner() != player_id) continue;
            r.setPosition(pos);
        }
        break;
    case PacketType::catch_troll:
        {
            RenderableID id = p.read<RenderableID>();
            Renderable& player = *m_mEntities[m_mPlayers[player_id].PlayerEntity];
            auto it = m_mEntities.find(id);
            if (it == m_mEntities.end()) {
                std::cout << "tried to catch not existing troll" << std::endl;
                break;
            }
            if ((player.getPosition() - it->second->getPosition()).magnitudeSquared() > 10*10) break;
            eraseEntity(id);
            m_mPlayers[player_id].TrollsCaught++;
            Packet p2;
            p2.write(PacketType::scoreboard);
            p2.write(player_id);
            p2.write<uint32_t>(m_mPlayers[player_id].TrollsCaught);
            sendPacketToAll(p2);
        }
        break;
    default:
        std::cout << "invalid packet type received: " << static_cast<uint32_t>(pt) << std::endl;
        break;
    }
    p.endRead();
}



void ServerPage::sendPacketToAll(const Packet& p)
{
    for (auto& ptr : m_sClients) {
        Gosu::CommSocket& cs = *(ptr.second);
        p.writeTo(cs);
        cs.sendPendingData();
    }
}

void ServerPage::PositionChanged(const Renderable& r)
{
    Packet p;
    p.write(PacketType::set_entity_position);
    p.write(r.getID());
    p.write(r.getPosition());
    sendPacketToAll(p);
}

Renderable& ServerPage::getEntity(RenderableID id)
{
    auto it = m_mEntities.find(id);
    assert(it != m_mEntities.end());
    return *(it->second);
}

void ServerPage::eraseEntity(RenderableID id)
{
    m_mEntities.erase(id);
    Packet p;
    p.write(PacketType::delete_entities);
    p.write(id);
    sendPacketToAll(p);
}
