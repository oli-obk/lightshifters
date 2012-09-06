#include "PacketType.h"
#include "packet.h"
#include "server_page.h"
#include "bullet.h"
#include "line.hpp"

Bullet::Bullet(Vector direction)
{
    direction.normalize();
    direction *= 50;
    m_vecDirection = direction;
    setType("bullet");
    setScale(0.001);
    m_uLifetime = 20;
}

Bullet::~Bullet()
{
}

void Bullet::update()
{
    ServerPage& page = ServerPage::getInstance();
    if (--m_uLifetime == 0) {
        page.eraseEntity(getID());
        return;
    }
    // check for hit
    Line line(getPosition(), m_vecDirection);
    optional<ServerEntity&> found;
    auto cb = [&found, this](ServerEntity& ent) -> bool
    {
        // don't hit myself
        if (ent.getID() == getID()) return true;
        // don't hit my player
        if (ent.getType() == "player") {
            if (ent.getOwner() == getOwner()) {
                return true;
            }
        }
        found.reset(ent);
        // stop
        return false;
    };
    page.intersect(line, cb);
    if (found) {
        Renderable& r = *found;
        page.bulletHit(*this, r);
        return;
    }
    // fly
    setPosition(getPosition()+m_vecDirection);
    Packet p;
    p.write(PacketType::set_entity_position);
    p.write(getID());
    p.write(getPosition());
    page.sendUdpPacketToAll(p);
}
