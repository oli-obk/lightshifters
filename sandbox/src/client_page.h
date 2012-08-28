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
    EntityMap m_mEntities;
    std::map<PlayerID, size_t> m_mTrollsCaught;
private:
    ClientPage(const ClientPage& rhs);
    ClientPage& operator=(const ClientPage& rhs);
    void onDisconnection();
    void onReceive(const void*, std::size_t);

    void firePlasma(Vector direction);

public:
    ClientPage(std::string addr, uint16_t port);
    ~ClientPage();
    void PositionChanged(const Renderable&);
    Renderable& getEntity(RenderableID id);
    void update();
    void draw();
};

#endif // CLIENTPAGE_H
