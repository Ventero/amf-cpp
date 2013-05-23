#pragma once
#ifndef AMF_HPP
#define AMF_HPP

#include <algorithm>
#ifdef _WIN32
	#define __LITTLE_ENDIAN 1234
	#define __BIG_ENDIAN 4321
	#define __BYTE_ORDER __LITTLE_ENDIAN
#else
	#include <endian.h>
#endif

typedef unsigned char u8;
typedef std::vector<u8> v8;

template <typename T>
T swap_endian(T x) {
	u8& bytes = reinterpret_cast<u8&>(x);
	std::reverse(&bytes, &bytes + sizeof(T));
	return x;
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define hton(x) swap_endian(x)
#define ntoh(x) swap_endian(x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define hton(x) (x)
#define ntoh(x) (x)
#else
#error Endianness not supported
#endif

#endif
