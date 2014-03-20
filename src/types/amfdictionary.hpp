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

#include "utils/amfitemptr.hpp"

namespace amf {
struct AmfDictionaryHash {
public:
	std::size_t operator()(const AmfItemPtr& val) const {
		auto s = val->serialize();
		return std::hash<std::string>()(std::string(s.begin(), s.end()));
	}
};

// Flash Player doesn't support deserializing booleans and number types
// (AmfInteger/AmfDouble), so we have to serialize them as strings
template<class T, bool>
struct AmfDictionaryKeyConverter {
	static AmfItemPtr convert(const T& item) {
		return AmfItemPtr(new T(item));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfInteger, true> {
	static AmfItemPtr convert(const AmfInteger& item) {
		std::string val = std::to_string(static_cast<int>(item));
		return AmfItemPtr(new AmfString(val));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfDouble, true> {
	static AmfItemPtr convert(const AmfDouble& item) {
		std::ostringstream str;
		str << std::setprecision(std::numeric_limits<double>::digits10)
		    << static_cast<double>(item);
		return AmfItemPtr(new AmfString(str.str()));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfBool, true> {
	static AmfItemPtr convert(const AmfBool& item) {
		return AmfItemPtr(new AmfString(item ? "true" : "false"));
	}
};

class AmfDictionary : public AmfItem {
public:
	AmfDictionary(bool numbersAsStrings, bool weak = false) :
		asString(numbersAsStrings), weak(weak) { };

	bool operator==(const AmfItem& other) const {
		const AmfDictionary* p = dynamic_cast<const AmfDictionary*>(&other);
		return p != nullptr && asString == p->asString && weak == p->weak &&
			values == p->values;
	}

	template<class T, class V>
	void insert(const T& key, const V& value) {
		static_assert(std::is_base_of<AmfItem, V>::value, "Values must extend AmfItem");

		(*this)[key].reset(new V(value));
	}

	template<class T, class V>
	T& at(const V& key) {
		auto ptr = (*this)[key];
		return *static_cast<T*>(ptr.get());
	}

	void clear() {
		values.clear();
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(values.size()).asLength(AMF_DICTIONARY);

		buf.push_back(weak ? 0x01 : 0x00);

		for (auto it : values) {
			auto k = it.first->serialize();
			auto v = it.second->serialize();
			buf.insert(buf.end(), k.begin(), k.end());
			buf.insert(buf.end(), v.begin(), v.end());
		}

		return buf;
	}

	bool asString;
	bool weak;
	std::unordered_map<
		AmfItemPtr,
		AmfItemPtr,
		AmfDictionaryHash
	> values;

private:
	template<class T>
	AmfItemPtr& operator[](const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Keys must extend AmfItem");

		auto val = asString ?
			AmfDictionaryKeyConverter<T, true>::convert(item) :
			AmfDictionaryKeyConverter<T, false>::convert(item);

		AmfItemPtr p(val);
		return values[p];
	}
};

} // namespace amf

#endif
