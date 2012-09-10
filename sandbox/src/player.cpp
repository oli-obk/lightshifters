#include "PacketType.h"
#include "packet.h"
#include "player.h"
#include "server_page.h"

Player::Player(PlayerID id)
{
	setScale(0.1);
	setType("player");
    setOwner(id);
}

Player::~Player()
{
}

void Player::fire(Vector direction)
{
    direction.normalize();
    ServerPage& page = ServerPage::getInstance();
    page.addBullet(getPosition(), direction, getOwner());
    Packet p;
    p.write(PacketType::create_bullet);
    p.write(getPosition());
    p.write(direction);
    p.write(getOwner());
}
