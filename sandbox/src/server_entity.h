#ifndef SERVERENTITY_H
#define SERVERENTITY_H

#include "renderable.h" // Base class: Renderable

class ServerEntity : public Renderable {

private:
	ServerEntity(const ServerEntity& rhs);
	ServerEntity& operator=(const ServerEntity& rhs);
protected:
	ServerEntity();
public:
	~ServerEntity();
    virtual void update();
};

#endif // SERVERENTITY_H
