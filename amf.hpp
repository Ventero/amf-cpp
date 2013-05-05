#pragma once
#ifndef AMF_HPP
#define AMF_HPP

#include <algorithm>
#include <endian.h>

typedef unsigned char u8;

// lololololololol
template <typename T>
T swap_endian(T x) {
	char& bytes = reinterpret_cast<char&>(x);
	std::reverse(&bytes, &bytes + sizeof(T));
	return x;
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define hton(x) swap_endian(x)
#define ntoh(x) swap_endian(x)
#else
#define hton(x) (x)
#define ntoh(x) (x)
#endif

#endif
