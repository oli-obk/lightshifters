#include <Gosu/Utility.hpp>
#include "PacketType.h"
#include "packet.h"
#include "renderable.h"
#include "client_page.h"
#include <sstream>

Renderable& createEntity(ClientPage::EntityMap& list, const Packet& p)
{
    Renderable r(p);
    RenderableID id = r.getID();
    auto it = list.insert(std::make_pair(id, std::move(r)));
    return it.first->second;
}

ClientPage::ClientPage(std::string addr, uint16_t port)
    :m_Connection(Gosu::cmManaged, Gosu::stringToAddress(addr), port)
{
    m_Connection.onDisconnection = std::bind(&ClientPage::onDisconnection, this);
    m_Connection.onReceive = std::bind(&ClientPage::onReceive, this, std::placeholders::_1, std::placeholders::_2);
}

ClientPage::~ClientPage()
{
}

void ClientPage::onDisconnection()
{
    std::cout << "lost connection to server" << std::endl;
}


void ClientPage::onReceive(const void* data, std::size_t size)
{
    Packet p(data, size);
    p.beginRead();
    PacketType pt = p.read<PacketType>();
    switch (pt) {
    case PacketType::create_entities:
        while (p.bytesLeftToRead()) {
            Renderable& r = createEntity(m_mEntities, p);
            if (r.getOwner() == m_pidMine) {
                m_pPlayerRenderable = &r;
            }
            std::cout << "new item of type: " << r.getType() << " at " << r.getPosition() << " with scale " << r.getScale() << std::endl;
            std::cout << Gosu::narrow(r.getImageName()) << std::endl;
        }
        break;
    case PacketType::delete_entities:
        while (p.bytesLeftToRead()) {
            RenderableID id = p.read<RenderableID>();
            auto it = m_mEntities.find(id);
            if (it == m_mEntities.end()) {
                std::cout << "tried to erase Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
                continue;
            }
            m_mEntities.erase(it);
        }
        break;
    case PacketType::set_entity_position:
        while (p.bytesLeftToRead()) {
            RenderableID id = p.read<RenderableID>();
            Vector pos = p.read<Vector>();
            auto it = m_mEntities.find(id);
            if (it == m_mEntities.end()) {
                std::cout << "tried to set position of Renderable " << id << ", but it does not seem to exist anymore" << std::endl;
                continue;
            }
            Renderable& r = it->second;
            r.setPosition(pos);
        }
        break;
    case PacketType::set_player_id:
        {
            PlayerID id = p.read<PlayerID>();
            if (m_pidMine != InvalidPlayerID) {
                std::cout << "warning, changing player id when already had a valid id" << std::endl;
            }
            m_pidMine = id;
        }
        break;
    case PacketType::scoreboard:
        while (p.bytesLeftToRead()) {
            PlayerID id = p.read<PlayerID>();
            size_t trolls = p.read<uint32_t>();
            m_mTrollsCaught[id] = trolls;
        }
        break;
    default:
        std::cout << "invalid packet type received: " << static_cast<uint32_t>(pt) << std::endl;
        break;
    }
    p.endRead();
}

void ClientPage::PositionChanged(const Renderable& r)
{
    Packet p;
    p.write(PacketType::set_entity_position);
    p.write(r.getID());
    p.write(r.getPosition());
    p.writeTo(m_Connection);
}

Renderable& ClientPage::getEntity(RenderableID id)
{
    auto it = m_mEntities.find(id);
    assert(it != m_mEntities.end());
    return it->second;
}

void ClientPage::update()
{
    SpacePage::update();
    if (m_Closest.m_bValid && m_Closest.m_dDistSquared < 10*10) {
        Packet p;
        p.write(PacketType::catch_troll);
        p.write(m_Closest.m_ID);
        p.writeTo(m_Connection);
    }
    m_Connection.update();
}

void ClientPage::draw()
{
    SpacePage::draw();
    for (auto& it: m_mEntities) {
        render(it.second);
    }
    {
        double pos = 10;
            for (auto& it: m_mTrollsCaught) {
            std::wstringstream wss;
            wss << L"Trolls caught: " << it.second;
            m_Font.draw(wss.str(), 10, pos, 10);
            pos += 15;
        }
	}
}
