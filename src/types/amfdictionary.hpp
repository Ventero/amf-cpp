#pragma once
#ifndef AMFDICTIONARY_HPP
#define AMFDICTIONARY_HPP

#include <functional>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_map>

#include "deserializationcontext.hpp"
#include "deserializer.hpp"

#include "types/amfbool.hpp"
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
		return ptr.template as<T>();
	}

	void clear() {
		values.clear();
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(values.size()).asLength(AMF_DICTIONARY);

		buf.push_back(weak ? 0x01 : 0x00);

		for (auto it : values) {
			// convert key's value to string if necessary
			auto k = serializeKey(it.first);
			auto v = it.second->serialize();
			buf.insert(buf.end(), k.begin(), k.end());
			buf.insert(buf.end(), v.begin(), v.end());
		}

		return buf;
	}

	static AmfDictionary deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end || *it++ != AMF_DICTIONARY)
			throw std::invalid_argument("AmfDictionary: Invalid type marker");

		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0x00)
			return ctx.getObject<AmfDictionary>(type >> 1);

		if (it == end)
			throw std::out_of_range("Not enough bytes for AmfDictionary");

		bool weak = (*it++ == 0x01);
		AmfDictionary ret(false, weak);
		size_t contextIndex = ctx.addObject<AmfDictionary>(ret);

		int size = type >> 1;
		for (int i = 0; i < size; ++i) {
			AmfItemPtr key = Deserializer::deserialize(it, end, ctx);
			AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
			ret.values[key] = val;
		}

		ctx.setObject<AmfDictionary>(contextIndex, ret);
		return ret;
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
		return values[AmfItemPtr(new T(item))];
	}

	// Flash Player doesn't support deserializing booleans and number types
	// (AmfInteger/AmfDouble), so we may have to serialize them as strings
	v8 serializeKey(const AmfItemPtr& key) const {
		if (!asString)
			return key->serialize();

		const AmfInteger* intval = key.asPtr<AmfInteger>();
		if (intval != nullptr) {
			std::string strval = std::to_string(intval->value);
			return AmfString(strval).serialize();
		}

		const AmfDouble* doubleval = key.asPtr<AmfDouble>();
		if (doubleval != nullptr) {
			std::ostringstream str;
			str << std::setprecision(std::numeric_limits<double>::digits10)
			    << doubleval->value;
			return AmfString(str.str()).serialize();
		}

		const AmfBool* boolval = key.asPtr<AmfBool>();
		if (boolval != nullptr)
			return AmfString(*boolval ? "true" : "false").serialize();

		return key->serialize();
	}
};

} // namespace amf

#endif
