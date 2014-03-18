#pragma once
#ifndef AMFDICTIONARY_HPP
#define AMFDICTIONARY_HPP

#include <functional>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {
struct AmfDictionaryHash {
public:
	std::size_t operator()(const std::shared_ptr<AmfItem>& val) const {
		auto s = val->serialize();
		return std::hash<std::string>()(std::string(s.begin(), s.end()));
	}
};

struct AmfDictionaryKeyEqual {
public:
	bool operator()(const std::shared_ptr<AmfItem>& lhs,
		const std::shared_ptr<AmfItem>& rhs) const {
		return lhs->serialize() == rhs->serialize();
	}
};

// Flash Player doesn't support deserializing booleans and number types
// (AmfInteger/AmfDouble), so we have to serialize them as strings
template<class T, bool>
struct AmfDictionaryKeyConverter {
	static std::shared_ptr<AmfItem> convert(const T& item) {
		return std::shared_ptr<AmfItem>(new T(item));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfInteger, true> {
	static std::shared_ptr<AmfItem> convert(const AmfInteger& item) {
		std::string val = std::to_string(static_cast<int>(item));
		return std::shared_ptr<AmfItem>(new AmfString(val));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfDouble, true> {
	static std::shared_ptr<AmfItem> convert(const AmfDouble& item) {
		std::ostringstream str;
		str << std::setprecision(std::numeric_limits<double>::digits10)
		    << static_cast<double>(item);
		return std::shared_ptr<AmfItem>(new AmfString(str.str()));
	}
};

template<>
struct AmfDictionaryKeyConverter<AmfBool, true> {
	static std::shared_ptr<AmfItem> convert(const AmfBool& item) {
		return std::shared_ptr<AmfItem>(new AmfString(item ? "true" : "false"));
	}
};

class AmfDictionary : public AmfItem {
public:
	AmfDictionary(bool numbersAsStrings, bool weak = false) :
		asString(numbersAsStrings), weak(weak) { };

	template<class T, class V>
	void insert(const T& key, const V& value) {
		static_assert(std::is_base_of<AmfItem, V>::value, "Values must extend AmfItem");

		std::shared_ptr<AmfItem> ptr(new V(value));
		(*this)[key].swap(ptr);
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
		std::shared_ptr<AmfItem>,
		std::shared_ptr<AmfItem>,
		AmfDictionaryHash,
		AmfDictionaryKeyEqual
	> values;

private:
	template<class T>
	std::shared_ptr<AmfItem>& operator[](const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Keys must extend AmfItem");

		auto val = asString ?
			AmfDictionaryKeyConverter<T, true>::convert(item) :
			AmfDictionaryKeyConverter<T, false>::convert(item);

		std::shared_ptr<AmfItem> p(val);
		return values[p];
	}
};

} // namespace amf

#endif
