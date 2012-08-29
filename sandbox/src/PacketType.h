#ifndef PACKETTYPE_H
#define PACKETTYPE_H

#include <cstdint>
enum class PacketType : uint8_t
{
    create_entities,
    delete_entities,
	set_entity_position,
    set_player_id,
    catch_troll,
    scoreboard,
    fire_plasma,
    udp_port_update
};

#endif // PACKETTYPE_H