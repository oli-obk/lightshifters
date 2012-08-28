#ifndef BULLET_H
#define BULLET_H

#include "server_entity.h" // Base class: ServerEntity

class Bullet : public ServerEntity {

private:
	Bullet(const Bullet& rhs);
	Bullet& operator=(const Bullet& rhs);
    Vector m_vecDirection;
    uint32_t m_uLifetime;
public:
	Bullet(Vector dir);
	~Bullet();
    void update();

};

#endif // BULLET_H
