#include "player.h"
#include "server_page.h"
#include "bullet.h"

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
    std::unique_ptr<Bullet> ent(new Bullet(direction));
    ent->setOwner(getOwner());
    ent->setPosition(getPosition());
    page.addEntity(std::move(ent));
}
