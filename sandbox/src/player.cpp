#include "player.h"

Player::Player(PlayerID id)
{
	setScale(0.1);
	setType("player");
    setOwner(id);
}

Player::~Player()
{
}
