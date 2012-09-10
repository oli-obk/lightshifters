#ifndef PACKETTYPE_H
#define PACKETTYPE_H

#include <cstdint>
enum class PacketType : uint8_t
{
    create_entities = 0x00,
    delete_entities = 0x01,
	set_entity_position = 0x02,
    set_player_id = 0x03,
    caught_troll = 0x04,
    scoreboard = 0x05,
    fire_plasma = 0x06,
    udp_port_update = 0x07,
    create_bullet = 0x08
};

#endif // PACKETTYPE_H