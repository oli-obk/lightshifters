#ifndef CLIENTPAGE_H
#define CLIENTPAGE_H

#include "RenderableID.h"
#include "space_page.h" // Base class: SpacePage
#include <Gosu/Sockets.hpp>
#include <map>

class ClientPage : public SpacePage
{
public:
    typedef std::map<RenderableID, Renderable> EntityMap;
    Gosu::CommSocket m_Connection;
    Gosu::MessageSocket m_MessageSocket;
    EntityMap m_mEntities;
    std::map<PlayerID, size_t> m_mTrollsCaught;
    Gosu::SocketPort m_Port;
private:
    ClientPage(const ClientPage& rhs);
    ClientPage& operator=(const ClientPage& rhs);
    void onDisconnection();
    void onReceive(const void*, std::size_t);
    void onReceiveUdp(Gosu::SocketAddress, Gosu::SocketPort, const void*, std::size_t);

    void firePlasma(Vector direction);
    void sendPacket(const Packet& p);
public:
    ClientPage(std::string addr, Gosu::SocketPort port, Gosu::SocketPort host_port);
    ~ClientPage();
    void PositionChanged(const Renderable&);
    Renderable& getEntity(RenderableID id);
    void update();
    void draw();
    void caughtTroll(RenderableID id);

};

#endif // CLIENTPAGE_H
