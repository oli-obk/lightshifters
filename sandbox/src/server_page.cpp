#include "bullet.h"
#include "PacketType.h"
#include "server_page.h"
#include "player.h"
#include "packet.h"
#include "troll.h"
#include "RenderableID.h"
#include <sstream>


template<class T, typename... Args>
T& ServerPage::createEntity(Args... args)
{
    std::unique_ptr<T> ptr(new T(args...));
    T& r = *ptr;
    m_mEntities.insert(std::make_pair(r.getID(), std::move(ptr)));
    Packet p;
    p.write(PacketType::create_entities);
    r.serialize(p);
    sendPacketToAll(p);
    return r;
}

ServerPage* ServerPage::s_pInstance = nullptr;
ServerPage& ServerPage::getInstance()
{
    assert(s_pInstance);
    return *s_pInstance;
}

ServerPage::ServerPage(uint16_t port)
    :m_ListenerSocket(port)
    ,m_MessageSocket(port)
{
    std::cout << "listening on tcp " << m_ListenerSocket.address() << ":" << m_ListenerSocket.port() << " and udp " << m_MessageSocket.port() << std::endl;
    assert(!s_pInstance);
    s_pInstance = this;
    m_pidNext = 1;
    m_pidMine = 0;
    m_ListenerSocket.onConnection = std::bind(&ServerPage::onConnection, this, std::placeholders::_1);
    generateSpace();
    Renderable& r = createEntity<Player>(m_pidMine);
    m_mPlayers[m_pidMine].PlayerEntity = r.getID();
    m_pPlayerRenderable = &r;
}

void ServerPage::onConnection(Gosu::Socket& sock)
{
    // create first, so it is not sent to the new player (new player receives all entities anyway)
    Renderable& r = createEntity<Player>(m_pidNext);
    
    // now add new connection
    auto pair = m_sClients.insert(std::make_pair(m_pidNext, new Gosu::CommSocket(Gosu::cmManaged, sock)));
    Gosu::CommSocket& cs = *(pair.first->second);
    cs.onDisconnection = std::bind(&ServerPage::onDisconnection, this, pair.first);
    cs.onReceive = std::bind(&ServerPage::onReceive, this, m_pidNext, std::placeholders::_1, std::placeholders::_2);
    std::cout << "new connection from: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;

    m_mPlayers[m_pidNext].PlayerEntity = r.getID();
    
    // notify the player of it's player id
    Packet p2;
    p2.write(PacketType::set_player_id);
    p2.write(m_pidNext);
    sendPacketTo(p2, m_pidNext);
    
    // send all entities
    Packet p;
    p.write(PacketType::create_entities);
for (auto& ptr: m_mEntities) {
        Renderable& r2 = *ptr.second;
        r2.serialize(p);
    }
    sendPacketTo(p, m_pidNext);
    
    // send scoreboard
    Packet p3;
    p3.write(PacketType::scoreboard);
for (auto& ptr: m_mPlayers) {
        p3.write(ptr.first);
        p3.write<uint32_t>(ptr.second.TrollsCaught);
    }
    sendPacketTo(p3, m_pidNext);
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
        Troll& troll = createEntity<Troll>();
        troll.setPosition(pos);
        troll.setColor(Temperature(temp).color());
    }
}

ServerPage::~ServerPage()
{
    assert(s_pInstance);
    s_pInstance = nullptr;
}

void ServerPage::update()
{
    SpacePage::update();
    m_ListenerSocket.update();
for(auto& it : m_sClients) {
        it.second->update();
    }
for(auto& it : m_mEntities) {
        it.second->update();
    }
    if (!m_pPlayerRenderable) return;
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
    case PacketType::catch_troll: {
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
    case PacketType::fire_plasma: {
        Vector dir = p.read<Vector>();
        dir.normalize();
        createEntity<Bullet>(dir).setPosition(m_mEntities[m_mPlayers[player_id].PlayerEntity]->getPosition());
    }
    default:
        std::cout << "invalid packet type received: " << static_cast<uint32_t>(pt) << std::endl;
        break;
    }
    p.endRead();
}

void ServerPage::send(const Packet& p, Gosu::CommSocket& cs)
{
    if (p.buflen() == 0) {
        std::cout << "tried to send zero length data" << std::endl;
        return;
    }
    if (p.sendByUdp()) {
        if (p.buflen() > m_MessageSocket.maxMessageSize()) {
            std::cout << "tried to send a packet by udp that surpasses maximum length of " << m_MessageSocket.maxMessageSize() << " bytes" << std::endl;
            return;
        }
        m_MessageSocket.send(cs.address(), cs.port(), p.buf(), p.buflen());
    } else {
        cs.send(p.buf(), p.buflen());
        cs.sendPendingData();
    }
}

void ServerPage::sendPacketTo(const Packet& p, PlayerID player)
{
    Gosu::CommSocket& cs = *m_sClients[player];
    send(p, cs);
}

void ServerPage::sendPacketToAll(const Packet& p, PlayerID exclude)
{
for (auto& ptr : m_sClients) {
        if (ptr.first == exclude) continue;
        Gosu::CommSocket& cs = *(ptr.second);
        send(p, cs);
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

void ServerPage::firePlasma(Vector direction)
{
    createEntity<Bullet>(direction).setPosition(m_pPlayerRenderable->getPosition());
}

void ServerPage::caughtTroll(RenderableID id)
{
    eraseEntity(id);
    m_mPlayers[m_pidMine].TrollsCaught++;
}
