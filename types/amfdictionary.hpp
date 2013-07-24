#pragma once
#ifndef AMFDICTIONARY_HPP
#define AMFDICTIONARY_HPP

#include "amfitem.hpp"
#include "amfinteger.hpp"
#include "amfstring.hpp"

#include <functional>
#include <unordered_map>

namespace std {
	template<>
	struct hash<std::vector<u8>> {
	public:
		std::size_t operator()(const std::vector<u8>& s) const {
			std::string chars(s.begin(), s.end());
			return std::hash<std::string>()(chars);
		}
	};
}

// Flash Player doesn't support deserializing booleans and number types
// (AmfInteger/AmfDouble), so we have to serialize them as strings
template<class T>
struct AmfDictionaryKeyConverter {
	static std::vector<u8> convert(const T& item) {
		return item.serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfInteger> {
	static std::vector<u8> convert(const AmfInteger& item) {
		std::string val = std::to_string(static_cast<int>(item));
		return AmfString(val).serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfDouble> {
	static std::vector<u8> convert(const AmfDouble& item) {
		std::string val = std::to_string(static_cast<double>(item));
		return AmfString(val).serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfBool> {
	static std::vector<u8> convert(const AmfBool& item) {
		return AmfString(item ? "true" : "false").serialize();
	}
};

class AmfDictionary : public AmfItem {
public:
	AmfDictionary(bool weak = false) : weak(weak) { };

	template<class T>
	std::vector<u8>& operator[](const T& item) {
		return values[AmfDictionaryKeyConverter<T>::convert(item)];
	}

	template<class T, class U>
	void insert(const T& key, const U& value) {
		(*this)[key] = value.serialize();
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(values.size()).asLength(AMF_DICTIONARY);

		buf.push_back(weak ? 0x01 : 0x00);

		for (auto it : values) {
			buf.insert(buf.end(), it.first.begin(), it.first.end());
			buf.insert(buf.end(), it.second.begin(), it.second.end());
		}

		return buf;
	}

private:
	bool weak;
	std::unordered_map<std::vector<u8>, std::vector<u8>> values;
};

#endif
