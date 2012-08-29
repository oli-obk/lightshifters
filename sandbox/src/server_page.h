#ifndef SERVERPAGE_H
#define SERVERPAGE_H

#include "space_page.h" // Base class: SpacePage
#include <Gosu/Sockets.hpp>
#include "RenderableID.h"
#include <memory>

struct ServerEntity;
struct Renderable;

#include <map>

struct PlayerState {
    PlayerState() = delete;
    PlayerState(std::unique_ptr<Gosu::CommSocket> sock):Socket(std::move(sock)) {}
    RenderableID PlayerEntity;
    size_t TrollsCaught;
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

    void caughtTroll(RenderableID id);

    template<class T, typename... Args>
    T& createEntity(Args... args);
    void send(const Packet& p, PlayerState&);
public:
    void sendPacketToAll(const Packet& p, PlayerID exclude = InvalidPlayerID);
    void sendPacketTo(const Packet& p, PlayerID player);
    ServerPage(uint16_t port);
    ~ServerPage();

    void PositionChanged(const Renderable&);
    Renderable& getEntity(RenderableID id);
    void eraseEntity(RenderableID id);

    void firePlasma(Vector direction);

};

#endif // SERVERPAGE_H
