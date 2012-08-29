#include "packet.h"

#include <iostream>

Packet::Packet(const void* data, size_t size)
:m_voidData(static_cast<const uint8_t*>(data))
,m_voidSize(size)
,m_bSendByUdp(false)
{
	m_bReadInProgress = false;
}

Packet::Packet(bool tcp)
:m_voidData(nullptr)
,m_voidSize(0)
,m_bSendByUdp(!tcp)
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

bool Packet::sendByUdp() const
{
    return m_bSendByUdp;
}
