#ifndef CLIENTPAGE_H
#define CLIENTPAGE_H

#include "RenderableID.h"
#include "space_page.h" // Base class: SpacePage
#include <Gosu/Sockets.hpp>
#include <map>

class ClientPage : public SpacePage
{
private:
    typedef std::map<RenderableID, Renderable> EntityMap;
    Renderable& createEntity(EntityMap& list, const Packet& p);
    Gosu::CommSocket m_Connection;
    Gosu::MessageSocket m_MessageSocket;
    EntityMap m_mEntities;
    std::map<PlayerID, int32_t> m_mScore;
    Gosu::SocketPort m_Port;
private:
    ClientPage(const ClientPage& rhs);
    ClientPage& operator=(const ClientPage& rhs);
    void onDisconnection();
    void onReceive(const void*, std::size_t);
    void onReceiveUdp(Gosu::SocketAddress, Gosu::SocketPort, const void*, std::size_t);

    void firePlasma(Vector direction);
    void sendTcpPacket(const Packet& p);
    void sendUdpPacket(const Packet& p);
public:
    ClientPage(std::string addr, Gosu::SocketPort port, Gosu::SocketPort host_port);
    ~ClientPage();
    void PositionChanged(const Renderable&);
    optional<Renderable&> getEntity(RenderableID id);
    void update();
    void draw();

};

#endif // CLIENTPAGE_H
