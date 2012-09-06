#ifndef SERVERPAGE_H
#define SERVERPAGE_H

#include "space_page.h" // Base class: SpacePage
#include <Gosu/Sockets.hpp>
#include "RenderableID.h"
#include <memory>
#include "optional.hpp"

class ServerEntity;
struct Renderable;

#include <map>

class Player;

struct PlayerState {
#ifdef WIN32
private:
    PlayerState();
public:
    // stupid msvc std pair wants a copy constructor, this works, too
    PlayerState(PlayerState&& other) {
        Score = other.Score;
        PlayerEntity = other.PlayerEntity;
        UdpPort = other.UdpPort;
        Socket = std::move(other.Socket);
    }
#else
    PlayerState() = delete;
#endif
    PlayerState(std::unique_ptr<Gosu::CommSocket> sock):Socket(std::move(sock)) {}
    optional<Player&> Entity;
    int32_t Score;
    uint16_t UdpPort;
    std::unique_ptr<Gosu::CommSocket> Socket;
};

class ServerPage : public SpacePage
{
private:
    static ServerPage* s_pInstance;
public:
    static ServerPage& getInstance();
private:
    Gosu::ListenerSocket m_ListenerSocket;
    Gosu::MessageSocket m_MessageSocket;
    void generateSpace();
    typedef std::map<RenderableID, std::unique_ptr<ServerEntity> > EntityMap;
    EntityMap m_mEntities;
    PlayerID m_pidNext;
    typedef std::map<PlayerID, PlayerState> PlayerMap;
    PlayerMap m_mPlayers;
private:
    ServerPage(const ServerPage& rhs);
    ServerPage& operator=(const ServerPage& rhs);
    void onDisconnection(PlayerMap::iterator);
    void onReceive(PlayerID, const void*, std::size_t);
    void onReceiveUdp(Gosu::SocketAddress, Gosu::SocketPort, const void*, std::size_t);
    void onConnection(Gosu::Socket& sock);
    void update();
    void draw();
    
    void sendUdpPacket(const Packet& p, PlayerState&);
    void sendTcpPacket(const Packet& p, PlayerState&);
#ifdef WIN32
    template<class T>
    T& createEntity();
    template<class T, class A>
    T& createEntity(A a);
    template<class T, class A, class B>
    T& createEntity(A a, B b);
#else
    template<class T, typename... Args>
    T& createEntity(Args... args);
#endif
public:
    void addEntity(std::unique_ptr<ServerEntity> ent);

    void sendUdpPacketToAll(const Packet& p, PlayerID exclude = InvalidPlayerID);
    void sendTcpPacketToAll(const Packet& p, PlayerID exclude = InvalidPlayerID);
    void sendUdpPacketTo(const Packet& p, PlayerID player);
    void sendTcpPacketTo(const Packet& p, PlayerID player);
    void bulletHit(ServerEntity& bullet, Renderable& target);
    ServerPage(uint16_t port);
    ~ServerPage();

    void PositionChanged(const Renderable&);
    optional<Renderable&> getEntity(RenderableID id);
    void eraseEntity(RenderableID id);
    void eraseEntity(EntityMap::iterator id);

    void firePlasma(Vector direction, Player& p);
    void firePlasma(Vector direction);
    optional<ServerEntity&> getClosestTo(Renderable& r, double maxdist);

};

#endif // SERVERPAGE_H
