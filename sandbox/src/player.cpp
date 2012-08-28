#include "player.h"

Player::Player(PlayerID id)
{
	setScale(0.01);
	setType("player");
    setOwner(id);
}

Player::~Player()
{
}
