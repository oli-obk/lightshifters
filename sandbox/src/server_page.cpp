#include "bullet.h"
#include "PacketType.h"
#include "server_page.h"
#include "player.h"
#include "packet.h"
#include "troll.h"
#include "RenderableID.h"
#include <sstream>
#include <random>
#include <Gosu/Math.hpp>

#ifdef WIN32
template<class T, class A>
T& ServerPage::createEntity(A a)
{
    std::unique_ptr<T> ptr(new T(a));
    T& r = *ptr;
    std::cout << "creating " << r.getID() << " of type " << r.getType() << std::endl;
    m_mEntities.insert(std::make_pair(r.getID(), std::move(ptr)));
    Packet p;
    p.write(PacketType::create_entities);
    r.serialize(p);
    sendPacketToAll(p);
    return r;
}

template<class T>
T& ServerPage::createEntity()
{
    std::unique_ptr<T> ptr(new T());
#else
template<class T, typename... Args>
T& ServerPage::createEntity(Args... args)
{
    std::unique_ptr<T> ptr(new T(args...));
#endif
    T& r = *ptr;
    std::cout << "creating " << r.getID() << " of type " << r.getType() << std::endl;
    m_mEntities.insert(std::make_pair(r.getID(), std::move(ptr)));
    Packet p;
    p.write(PacketType::create_entities);
    r.serialize(p);
    sendTcpPacketToAll(p);
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
    using namespace std::placeholders;
    m_ListenerSocket.onConnection = std::bind(&ServerPage::onConnection, this, _1);
    m_MessageSocket.onReceive = std::bind(&ServerPage::onReceiveUdp, this, _1, _2, _3, _4);
    generateSpace();
    Player& r = createEntity<Player>(m_pidMine);
    PlayerState player(std::unique_ptr<Gosu::CommSocket>(nullptr));
    player.Entity.reset(r);
    player.Score = 0;
    m_mPlayers.insert(std::make_pair(m_pidMine, std::move(player)));
    m_PlayerRenderable.reset(r);
}

void ServerPage::onConnection(Gosu::Socket& sock)
{
    // create first, so it is not sent to the new player (new player receives all entities anyway)
    Player& r = createEntity<Player>(m_pidNext);

    // now add new connection
    PlayerMap::iterator it;
    {
        PlayerState player(std::unique_ptr<Gosu::CommSocket>(new Gosu::CommSocket(Gosu::cmManaged, sock)));
        player.Score = 0;
        player.UdpPort = 0;
        player.Entity.reset(r);
        auto pair = m_mPlayers.insert(std::make_pair(m_pidNext, std::move(player)));
        assert(pair.second);
        it = pair.first;
    }
    Gosu::CommSocket& cs = *(it->second.Socket);
    cs.onDisconnection = std::bind(&ServerPage::onDisconnection, this, it);
    cs.onReceive = std::bind(&ServerPage::onReceive, this, m_pidNext, std::placeholders::_1, std::placeholders::_2);
    std::cout << "new connection from: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;

    // notify the player of it's player id
    Packet p2;
    p2.write(PacketType::set_player_id);
    p2.write(m_pidNext);
    sendTcpPacketTo(p2, m_pidNext);

    // send all entities
    Packet p;
    p.write(PacketType::create_entities);
    for (auto& ptr: m_mEntities) {
        Renderable& r2 = *ptr.second;
        r2.serialize(p);
    }
    sendTcpPacketTo(p, m_pidNext);

    // send scoreboard
    Packet p3;
    p3.write(PacketType::scoreboard);
    for (auto& ptr: m_mPlayers) {
        p3.write(ptr.first);
        p3.write<uint32_t>(ptr.second.Score);
    }
    sendTcpPacketTo(p3, m_pidNext);
    cs.sendPendingData();
    m_pidNext++;
}

void ServerPage::generateSpace()
{
    std::uniform_real_distribution<double> posgen(-1000, 1000);
    std::uniform_int_distribution<uint32_t> tempgen(0, 60000);
    std::mt19937 engine; // Mersenne twister MT19937
    for (int i = 0; i < 10; i++) {
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
    m_MessageSocket.update();
    for(auto& it : m_mPlayers) {
        if (!it.second.Socket) continue;
        it.second.Socket->update();
    }
    for(auto it = m_mEntities.begin(); it != m_mEntities.end();)
    {
        // copy necessary to allow update to delete the entity
        auto entit = it++;
        entit->second->update();
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
                wss << L"You: ";
            } else {
                wss << L"Player " << it.first << ": ";
            }
            wss << it.second.Score;
            m_Font.draw(wss.str(), 10, pos, 10);
            pos += 15;
        }
    }
}



void ServerPage::onDisconnection(PlayerMap::iterator connit)
{
    // the Socket is already invalid, DO NOT ACCESS INSIDE THIS FUNCTION
    //const Gosu::CommSocket& cs = **it;
    //std::cout << "lost connection to: " << Gosu::addressToString(cs.address()) << ":" << cs.port() << std::endl;
    PlayerID pid = connit->first;
    for (auto it = m_mEntities.begin(); it != m_mEntities.end();) {
        auto to_erase = it++;
        assert(to_erase->second);
        Renderable& r = *(to_erase->second);
        if (r.getOwner() != pid) continue;
        eraseEntity(to_erase);
    }
    m_mPlayers.erase(connit);
}

void ServerPage::onReceiveUdp(Gosu::SocketAddress addr, Gosu::SocketPort port, const void* data, std::size_t size)
{
    PlayerID player_id = InvalidPlayerID;
    //std::cout << "udp packet from: " << Gosu::addressToString(addr) << ":" << port << std::endl;
    PlayerState* pPlayer = nullptr;
    for(auto& it : m_mPlayers) {
        if (!it.second.Socket) continue;
        if (it.second.Socket->address() != addr) continue;
        if (it.second.UdpPort != port) continue;
        player_id = it.first;
        pPlayer = &it.second;
        break;
    }
    if (player_id == InvalidPlayerID) {
        std::cout << "some hotshot sent us a udp packet but is not in our game" << std::endl;
        std::cout << "source: " << Gosu::addressToString(addr) << ":" << port << std::endl;
        std::cout << "content: " << std::string(static_cast<const char*>(data), static_cast<const char*>(data)+size) << std::endl;
        return;
    }
    PlayerState& player = *pPlayer;
    Packet p(data, size);
    p.beginRead();
    PacketType pt = p.read<PacketType>();
    switch (pt) {
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
            // i only accept position commands for player entities
            if (r.getType() != "player") continue;
            r.setPosition(pos);
            sendUdpPacketToAll(p, player_id);
        }
        break;
    case PacketType::catch_troll: {
        RenderableID id = p.read<RenderableID>();
        auto playerit = m_mPlayers.find(player_id);
        assert(playerit != m_mPlayers.end());

        Renderable& playerentity = *(player.Entity);
        auto it = m_mEntities.find(id);
        if (it == m_mEntities.end()) {
            std::cout << "tried to catch not existing troll" << std::endl;
            break;
        }
        if ((playerentity.getPosition() - it->second->getPosition()).magnitudeSquared() > 10*10) break;
        eraseEntity(id);
        player.Score++;
        Packet p2;
        p2.write(PacketType::scoreboard);
        p2.write(player_id);
        p2.write(player.Score);
        sendTcpPacketToAll(p2);
    }
    break;
    case PacketType::fire_plasma:
    {
        Vector dir = p.read<Vector>();
        dir.normalize();
        firePlasma(player.Entity->getPosition(), dir, player_id);
    }
    break;
    default:
        std::cout << "invalid packet or not meant to be sent by udp" << std::endl;
        break;
    }
}

void ServerPage::onReceive(PlayerID player_id, const void* data, std::size_t size)
{
    auto playerit = m_mPlayers.find(player_id);
    assert(playerit != m_mPlayers.end());
    PlayerState& player = playerit->second;
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
    case PacketType::udp_port_update:
    {
        uint16_t port = p.read<uint16_t>();
        player.UdpPort = port;
    }
    break;
    default:
        std::cout << "invalid packet or not meant to be sent by tcp" << std::endl;
        break;
    }
    p.endRead();
}

void ServerPage::sendTcpPacket(const Packet& p, PlayerState& player)
{
    if (p.buflen() == 0) {
        std::cout << "tried to send zero length data" << std::endl;
        return;
    }
    player.Socket->send(p.buf(), p.buflen());
    player.Socket->sendPendingData();
}

void ServerPage::sendUdpPacket(const Packet& p, PlayerState& player)
{
    if (p.buflen() == 0) {
        std::cout << "tried to send zero length data" << std::endl;
        return;
    }
    if (p.buflen() > m_MessageSocket.maxMessageSize()) {
        std::cout << "tried to send a packet by udp that surpasses maximum length of " << m_MessageSocket.maxMessageSize() << " bytes" << std::endl;
        return;
    }
    m_MessageSocket.send(player.Socket->address(), player.UdpPort, p.buf(), p.buflen());
}

void ServerPage::sendTcpPacketTo(const Packet& p, PlayerID playerid)
{
    auto it = m_mPlayers.find(playerid);
    assert(it != m_mPlayers.end());
    sendTcpPacket(p, it->second);
}

void ServerPage::sendUdpPacketTo(const Packet& p, PlayerID playerid)
{
    auto it = m_mPlayers.find(playerid);
    assert(it != m_mPlayers.end());
    sendUdpPacket(p, it->second);
}

void ServerPage::sendTcpPacketToAll(const Packet& p, PlayerID exclude)
{
    for (auto& ptr : m_mPlayers) {
        if (!ptr.second.Socket) continue;
        if (ptr.first == exclude) continue;
        sendTcpPacket(p, ptr.second);
    }
}

void ServerPage::sendUdpPacketToAll(const Packet& p, PlayerID exclude)
{
    for (auto& ptr : m_mPlayers) {
        if (!ptr.second.Socket) continue;
        if (ptr.first == exclude) continue;
        sendUdpPacket(p, ptr.second);
    }
}

void ServerPage::PositionChanged(const Renderable& r)
{
    Packet p;
    p.write(PacketType::set_entity_position);
    p.write(r.getID());
    p.write(r.getPosition());
    sendUdpPacketToAll(p);
}

optional<Renderable&> ServerPage::getEntity(RenderableID id)
{
    auto it = m_mEntities.find(id);
    assert(it != m_mEntities.end());
    return optional<Renderable&>(static_cast<Renderable&>(*(it->second)));
}

void ServerPage::eraseEntity(RenderableID id)
{
    eraseEntity(m_mEntities.find(id));
}


void ServerPage::firePlasma(Vector direction)
{
    firePlasma(m_PlayerRenderable->getPosition(), direction, m_pidMine);
}

void ServerPage::firePlasma(Vector pos, Vector direction, PlayerID pid)
{
    Bullet& bullet = createEntity<Bullet>(direction);
    std::cout << pid << " fired a bullet" << std::endl;
    bullet.setPosition(pos);
    bullet.setOwner(pid);
}

void ServerPage::caughtTroll(RenderableID id)
{
    eraseEntity(id);
    auto it = m_mPlayers.find(m_pidMine);
    assert(it != m_mPlayers.end());
    it->second.Score++;
}

optional<ServerEntity&> ServerPage::getClosestTo(Renderable& r, double maxdist)
{
    double maxsq = maxdist*maxdist;
    ClosestHud closest;
    for(auto& it: m_mEntities)
    {
        // do not check self
        if (it.first == r.getID()) continue;
        closest.check(*it.second, r.getPosition());
    }
    if (!closest.m_bValid) return optional<ServerEntity&>();
    if (closest.m_dDistSquared > maxsq) return optional<ServerEntity&>();
    return optional<ServerEntity&>(*m_mEntities[closest.m_ID]);
}

void ServerPage::bulletHit(ServerEntity& bullet, Renderable& target)
{
    // only hit players
    if (target.getType() != "player") return;
    // do not self-hit
    if (target.getOwner() == bullet.getOwner()) return;
    auto looser = m_mPlayers.find(target.getOwner());
    auto winner = m_mPlayers.find(bullet.getOwner());
    if (looser == m_mPlayers.end()) {
        std::cout << "bullet hit nonexisting player" << std::endl;
        return;
    }
    if (winner == m_mPlayers.end()) {
        std::cout << "bullet of nonexisting player hit existing player" << std::endl;
        return;
    }
    std::cout << winner->first << "'s bullet hit " << looser->first << std::endl;
    looser->second.Score--;
    winner->second.Score++;
    Packet p2;
    p2.write(PacketType::scoreboard);
    p2.write(looser->first);
    p2.write(looser->second.Score);
    p2.write(winner->first);
    p2.write(winner->second.Score);
    sendTcpPacketToAll(p2);
    eraseEntity(bullet.getID());
}

void ServerPage::eraseEntity(EntityMap::iterator it)
{
    assert(it != m_mEntities.end());
    RenderableID id = it->first;
    std::cout << "erasing " << id << std::endl;
    m_mEntities.erase(it);
    Packet p;
    p.write(PacketType::delete_entities);
    p.write(id);
    sendTcpPacketToAll(p);
}
