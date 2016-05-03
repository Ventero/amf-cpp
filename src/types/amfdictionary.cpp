#include "amfdictionary.hpp"

#include <iomanip>
#include <limits>
#include <sstream>

#include "deserializer.hpp"
#include "serializationcontext.hpp"
#include "types/amfbool.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"

namespace amf {

size_t AmfDictionaryHash::operator()(const AmfItemPtr& val) const {
	// TODO: use new context here?
	// TODO: maybe use slightly more performant hashing method instead ...
	SerializationContext ctx;
	auto s = val->serialize(ctx);
	return std::hash<std::string>()(std::string(s.begin(), s.end()));
}

bool AmfDictionary::operator==(const AmfItem& other) const {
	const AmfDictionary* p = dynamic_cast<const AmfDictionary*>(&other);
	return p != nullptr && asString == p->asString && weak == p->weak &&
		values == p->values;
}

std::vector<u8> AmfDictionary::serialize(SerializationContext & ctx) const {
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_DICTIONARY, u8(index << 1) };
	ctx.addObject(*this);

	std::vector<u8> buf = AmfInteger::asLength(values.size(), AMF_DICTIONARY);

	buf.push_back(weak ? 0x01 : 0x00);

	for (auto it : values) {
		// convert key's value to string if necessary
		auto k = serializeKey(it.first, ctx);
		auto v = it.second->serialize(ctx);
		buf.insert(buf.end(), k.begin(), k.end());
		buf.insert(buf.end(), v.begin(), v.end());
	}

	return buf;
}

AmfItemPtr AmfDictionary::deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_DICTIONARY)
		throw std::invalid_argument("AmfDictionary: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0x00)
		return ctx.getPointer<AmfDictionary>(type >> 1);

	if (it == end)
		throw std::out_of_range("Not enough bytes for AmfDictionary");

	bool weak = (*it++ != 0x00);

	AmfItemPtr ptr(new AmfDictionary(false, weak));
	AmfDictionary & dict = ptr.as<AmfDictionary>();
	ctx.addPointer(ptr);

	int size = type >> 1;
	for (int i = 0; i < size; ++i) {
		AmfItemPtr key = Deserializer::deserialize(it, end, ctx);
		AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
		dict.values[key] = val;
	}

	return ptr;
}

AmfDictionary AmfDictionary::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	return deserializePtr(it, end, ctx).as<AmfDictionary>();
}

v8 AmfDictionary::serializeKey(const AmfItemPtr& key, SerializationContext& ctx) const {
	if (!asString)
		return key->serialize(ctx);

	const AmfInteger* intval = key.asPtr<AmfInteger>();
	if (intval != nullptr) {
		std::string strval = std::to_string(intval->value);
		return AmfString(strval).serialize(ctx);
	}

	const AmfDouble* doubleval = key.asPtr<AmfDouble>();
	if (doubleval != nullptr) {
		std::ostringstream str;
		str << std::setprecision(std::numeric_limits<double>::digits10)
		    << doubleval->value;
		return AmfString(str.str()).serialize(ctx);
	}

	const AmfBool* boolval = key.asPtr<AmfBool>();
	if (boolval != nullptr)
		return AmfString(boolval->value ? "true" : "false").serialize(ctx);

	const AmfUndefined* undefinedval = key.asPtr<AmfUndefined>();
	if (undefinedval != nullptr)
		return AmfString("undefined").serialize(ctx);

	const AmfNull* nullval = key.asPtr<AmfNull>();
	if (nullval != nullptr)
		return AmfString("null").serialize(ctx);

	return key->serialize(ctx);
}

} // namespace amf
