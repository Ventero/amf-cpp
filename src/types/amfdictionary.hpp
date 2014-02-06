#pragma once
#ifndef AMFDICTIONARY_HPP
#define AMFDICTIONARY_HPP

#include <functional>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_map>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace std {
	template<>
	struct hash<amf::v8> {
	public:
		std::size_t operator()(const amf::v8& s) const {
			std::string chars(s.begin(), s.end());
			return std::hash<std::string>()(chars);
		}
	};
}

namespace amf {

// Flash Player doesn't support deserializing booleans and number types
// (AmfInteger/AmfDouble), so we have to serialize them as strings
template<class T>
struct AmfDictionaryKeyConverter {
	static std::vector<u8> convert(const T& item, bool) {
		return item.serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfInteger> {
	static std::vector<u8> convert(const AmfInteger& item, bool asString) {
		if(!asString) return item.serialize();

		std::string val = std::to_string(static_cast<int>(item));
		return AmfString(val).serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfDouble> {
	static std::vector<u8> convert(const AmfDouble& item, bool asString) {
		if(!asString) return item.serialize();

		std::ostringstream str;
		str << std::setprecision(std::numeric_limits<double>::digits10)
		    << static_cast<double>(item);
		return AmfString(str.str()).serialize();
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfBool> {
	static std::vector<u8> convert(const AmfBool& item, bool asString) {
		if(!asString) return item.serialize();

		return AmfString(item ? "true" : "false").serialize();
	}
};

class AmfDictionary : public AmfItem {
public:
	AmfDictionary(bool numbersAsStrings, bool weak = false) :
		asString(numbersAsStrings), weak(weak) { };

	template<class T>
	std::vector<u8>& operator[](const T& item) {
		return values[AmfDictionaryKeyConverter<T>::convert(item, asString)];
	}

	template<class T>
	void insert(const T& key, const AmfItem& value) {
		(*this)[key] = value.serialize();
	}

	void clear() {
		values.clear();
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
	bool asString;
	bool weak;
	std::unordered_map<std::vector<u8>, std::vector<u8>> values;
};

} // namespace amf

#endif
