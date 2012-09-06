#ifndef PLAYER_H
#define PLAYER_H

#include "PlayerID.h"
#include "server_entity.h"


class Player : public ServerEntity {

public:
	Player(PlayerID);
	~Player();
    void fire(Vector direction);
};

#endif // PLAYER_H
