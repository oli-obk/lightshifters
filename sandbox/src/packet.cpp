#include "packet.h"

#include <iostream>

Packet::Packet(const void* data, size_t size)
:m_voidData(static_cast<const uint8_t*>(data))
,m_voidSize(size)
{
	m_bReadInProgress = false;
}

Packet::~Packet()
{
}

const void* Packet::buf() const
{
	if (m_voidData) return static_cast<const void*>(m_voidData);
	return static_cast<const void*>(m_vData.data());
}

std::size_t Packet::buflen() const
{
	if (m_voidData) return m_voidSize;
	return m_vData.size();
}

void Packet::writeTo(Gosu::CommSocket& cs) const
{
	if (buflen() == 0) {
		std::cout << "tried to send zero length data" << std::endl;
		return;
	}
	cs.send(buf(), buflen()); 
}

void Packet::beginRead() const
{
	assert (!m_bReadInProgress);
	m_uCurReadPos = 0;
	m_bReadInProgress = true;
}

void Packet::endRead() const
{
	assert (m_bReadInProgress);
}

size_t Packet::bytesLeftToRead() const
{
	assert (m_bReadInProgress);
	return (buflen() - m_uCurReadPos);
}
