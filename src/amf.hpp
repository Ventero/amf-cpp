#pragma once
#ifndef AMF_HPP
#define AMF_HPP

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
	// Windows only runs on little endian platforms
	#define LITTLE_ENDIAN 1234
	#define BIG_ENDIAN 4321
	#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__APPLE__)
	#include <machine/endian.h>
#else
	#include <endian.h>
#endif

namespace amf {

typedef unsigned char u8;
typedef std::vector<u8> v8;

template <typename T>
T swap_endian(T x) {
	u8* bytes = reinterpret_cast<u8 *>(&x);
	std::reverse(bytes, bytes + sizeof(T));
	return x;
}

#if BYTE_ORDER == LITTLE_ENDIAN
#define hton(x) swap_endian(x)
#define ntoh(x) swap_endian(x)
#elif BYTE_ORDER == BIG_ENDIAN
#define hton(x) (x)
#define ntoh(x) (x)
#else
#error Endianness not supported
#endif

template <typename T>
v8 network_bytes(T x) {
	T swapped = hton(x);
	const u8* bytes = reinterpret_cast<const u8*>(&swapped);
	return v8(bytes, bytes + sizeof(T));
}

template<typename T>
T read_network(v8::const_iterator& it, v8::const_iterator end) {
	if (static_cast<size_t>(end - it) < sizeof(T))
		throw std::out_of_range("Not enough bytes to read");

	T val;
	std::copy(it, it + sizeof(T), reinterpret_cast<u8 *>(&val));
	it += sizeof(T);

	return ntoh(val);
}

} // namespace amf

#endif
