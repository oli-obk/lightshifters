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
    RenderableID PlayerEntity;
    size_t TrollsCaught;
};

class ServerPage : public SpacePage
{
private:
    static ServerPage* s_pInstance;
public:
    static ServerPage& getInstance();
private:
    Gosu::ListenerSocket m_ListenerSocket;
    void generateSpace();
    typedef std::map<PlayerID, std::unique_ptr<Gosu::CommSocket> > SocketSet;
    SocketSet m_sClients;
    typedef std::map<RenderableID, std::unique_ptr<ServerEntity> > EntityMap;
    EntityMap m_mEntities;
    PlayerID m_pidNext;
    std::map<PlayerID, PlayerState> m_mPlayers;
private:
    ServerPage(const ServerPage& rhs);
    ServerPage& operator=(const ServerPage& rhs);
    void onDisconnection(SocketSet::iterator);
    void onReceive(PlayerID, const void*, std::size_t);
    void onConnection(Gosu::Socket& sock);
    void update();
    void draw();

    template<class T, typename... Args>
    T& createEntity(Args... args);
public:
    void sendPacketToAll(const Packet& p);
    ServerPage(uint16_t port);
    ~ServerPage();

    void PositionChanged(const Renderable&);
    Renderable& getEntity(RenderableID id);
    void eraseEntity(RenderableID id);

    void firePlasma(Vector direction);

};

#endif // SERVERPAGE_H
