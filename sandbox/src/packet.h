#ifndef PACKET_H
#define PACKET_H

#include <Gosu/Platform.hpp>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <stdexcept>
#include "Vector.h"

template< class FROM, class TO, class Enable = void > struct copy_sign;

template< class FROM, class TO >
struct copy_sign<FROM, TO, typename std::enable_if<std::is_signed<FROM>::value>::type> : public std::make_signed<TO> {};

template< class FROM, class TO >
struct copy_sign<FROM, TO, typename std::enable_if<std::is_unsigned<FROM>::value>::type> : public std::make_unsigned<TO> {};

template<size_t size> struct get_int_type_of_size;

template<> struct get_int_type_of_size<1>{ typedef uint8_t type; };
template<> struct get_int_type_of_size<2>{ typedef uint16_t type; };
template<> struct get_int_type_of_size<4>{ typedef uint32_t type; };
template<> struct get_int_type_of_size<8>{ typedef uint64_t type; };
//template<> struct get_int_type_of_size<16>{ typedef uint128_t type; };

class Packet {

private:
	Packet(const Packet& rhs);
	Packet& operator=(const Packet& rhs);
	std::vector<uint8_t> m_vData;
	const uint8_t* m_voidData;
	const size_t m_voidSize;
	mutable size_t m_uCurReadPos;
	mutable bool m_bReadInProgress;
public:
    // receive
	Packet(const void* data, size_t size);
    Packet();
	~Packet();
	const void* buf() const;
	std::size_t buflen() const;
	void beginRead() const;
	void endRead() const;
	size_t bytesLeftToRead() const;
	template<typename T> T read(typename std::enable_if<std::is_pointer<T>::value>::type* p = nullptr) const
	{
		static_assert(!std::is_pointer<T>::value, "You cannot read pointers from a network package!");
	}
	template<typename T> void write(T value, typename std::enable_if<std::is_pointer<T>::value>::type* p = nullptr)
	{
		static_assert(!std::is_pointer<T>::value, "You cannot write pointers to a network package!");
	}
	template<typename T> T read(typename std::enable_if<std::is_enum<T>::value>::type* = nullptr) const
	{
		typedef typename get_int_type_of_size<sizeof(T)>::type T2;
		return static_cast<T>(read<T2>());
	}
	template<typename T> void write(T value, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr)
	{
		typedef typename get_int_type_of_size<sizeof(T)>::type T2;
		return write(static_cast<T2>(value));
	}
	template<typename T> T read(typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr) const
	{
		union {
			T value;
			char data[sizeof(T)];
		} un;
		const uint8_t* data = m_voidData?m_voidData:m_vData.data();
		assert(m_bReadInProgress);
		assert (m_uCurReadPos + sizeof(T) <= buflen());
		if (m_uCurReadPos + sizeof(T) > buflen()) {
			throw std::logic_error("tried to read more bytes from a Packet than are available");
		}
		#define _SCL_SECURE_NO_WARNINGS
		std::copy(data+m_uCurReadPos, data+m_uCurReadPos+sizeof(T), std::begin(un.data));
		#undef _SCL_SECURE_NO_WARNINGS
		m_uCurReadPos += sizeof(T);
		return Gosu::littleToNative(un.value);
	}
	template<typename T> void write(T value, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
	{
		assert(!m_voidData);
		union {
			T value;
			char data[sizeof(T)];
		} un;
		un.value = Gosu::nativeToLittle(value);
		assert(!m_bReadInProgress);
		m_vData.insert(std::end(m_vData), std::begin(un.data), std::end(un.data));
	}
	template<typename T> T read(typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr) const
	{
		const uint8_t* data = m_voidData?m_voidData:m_vData.data();
		const uint8_t len = read<uint8_t>();
		assert(m_bReadInProgress);
		assert (m_uCurReadPos + len <= buflen());
		if (m_uCurReadPos + len > buflen()) {
			throw std::logic_error("tried to read more bytes from a Packet than are available");
		}
		std::vector<uint8_t> str(data+m_uCurReadPos, data+m_uCurReadPos+len);
		m_uCurReadPos += len;
		std::string ret(str.begin(), str.end());
		return ret;
	}
	template<typename T> void write(T value, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr)
	{
		assert(!m_voidData);
		assert(!m_bReadInProgress);
		assert(value.size() <= 255);
		write<uint8_t>(value.size());
		m_vData.insert(std::end(m_vData), std::begin(value), std::end(value));
	}
	template<typename T> T read(typename std::enable_if<std::is_same<T, Vector>::value>::type* = nullptr) const
	{
		Vector ret;
		ret.x = read<double>();
		ret.y = read<double>();
		ret.z = read<double>();
		return ret;
	}
	template<typename T> void write(T value, typename std::enable_if<std::is_same<T, Vector>::value>::type* = nullptr)
	{
		write(value.x);
		write(value.y);
		write(value.z);
	}
};

#endif // PACKET_H
