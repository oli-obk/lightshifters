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

ClientPage::ClientPage(std::string addr, uint16_t port, uint16_t host_port)
    :m_Connection(Gosu::cmManaged, Gosu::stringToAddress(addr), port)
    ,m_MessageSocket(host_port)
{
    std::cout << "listening on udp " << host_port << std::endl;
    std::cout << "connected to tcp " << m_Connection.address() << ":" << m_Connection.port() << std::endl;
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
    case PacketType::set_player_id: {
        PlayerID id = p.read<PlayerID>();
        if (m_pidMine != InvalidPlayerID) {
            std::cout << "warning, changing player id when already had a valid id" << std::endl;
        }
        std::cout << "My Player id is now: " << id << std::endl;
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
    sendPacket(p);
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
            if (it.first == m_pidMine) {
                wss << L"You";
            } else {
                wss << L"Player " << it.first;
            }
            wss << L" caught " << it.second << L" Troll";
            if (it.second != 1) {
                wss << L"s";
            }
            m_Font.draw(wss.str(), 10, pos, 10);
            pos += 15;
        }
    }
}

void ClientPage::firePlasma(Vector direction)
{
    Packet p;
    p.write(PacketType::fire_plasma);
    p.write(direction);
    sendPacket(p);
}

void ClientPage::sendPacket(const Packet& p)
{
    if (p.buflen() == 0) {
        std::cout << "tried to send zero length data" << std::endl;
        return;
    }
    if (p.sendByUdp()) {
        if (p.buflen() > m_MessageSocket.maxMessageSize()) {
            std::cout << "tried to send a packet by udp that surpasses maximum length of " << m_MessageSocket.maxMessageSize() << " bytes" << std::endl;
            return;
        }
        m_MessageSocket.send(m_Connection.address(), m_Connection.port(), p.buf(), p.buflen());
    } else {
        m_Connection.send(p.buf(), p.buflen());
        m_Connection.sendPendingData();
    }
}

void ClientPage::caughtTroll(RenderableID id)
{
    Packet p;
    p.write(PacketType::catch_troll);
    p.write(id);
    sendPacket(p);
}
