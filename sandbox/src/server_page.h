#ifndef SERVERPAGE_H
#define SERVERPAGE_H

#include "space_page.h" // Base class: SpacePage
#include <Gosu/Sockets.hpp>
#include "RenderableID.h"
#include <memory>

struct Renderable;

#include <map>

struct PlayerState
{
    RenderableID PlayerEntity;
    size_t TrollsCaught;
};

class ServerPage : public SpacePage
{

    Gosu::ListenerSocket m_ListenerSocket;
    void generateSpace();
    typedef std::map<PlayerID, std::unique_ptr<Gosu::CommSocket> > SocketSet;
    SocketSet m_sClients;
    typedef std::map<RenderableID, std::unique_ptr<Renderable> > EntityMap;
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
    void sendPacketToAll(const Packet& p);
public:
    ServerPage(uint16_t port);
    ~ServerPage();

    void PositionChanged(const Renderable&);
    Renderable& getEntity(RenderableID id);
    void eraseEntity(RenderableID id);

};

#endif // SERVERPAGE_H
