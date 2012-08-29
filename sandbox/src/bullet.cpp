#include "PacketType.h"
#include "packet.h"
#include "server_page.h"
#include "bullet.h"

Bullet::Bullet(Vector direction)
{
    m_vecDirection = direction;
    setType("bullet");
    setScale(0.001);
    m_uLifetime = 1000;
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
    setPosition(getPosition()+m_vecDirection);
    Packet p(false);
    p.write(PacketType::set_entity_position);
    p.write(getID());
    p.write(getPosition());
    page.sendPacketToAll(p);
}
