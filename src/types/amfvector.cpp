#include "amfvector.hpp"

#include "deserializer.hpp"
#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {

template<typename T>
bool AmfVector<T, typename VectorProperties<T>::type>::operator==(const AmfItem& other) const {
	const AmfVector<T>* p = dynamic_cast<const AmfVector<T>*>(&other);
	return p != nullptr && fixed == p->fixed && values == p->values;
}

template<typename T>
std::vector<u8> AmfVector<T, typename VectorProperties<T>::type>::serialize(SerializationContext& ctx) const {
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { VectorProperties<T>::marker, u8(index << 1) };
	ctx.addObject(*this);

	// U29V value
	std::vector<u8> buf = AmfInteger::asLength(values.size(),
		VectorProperties<T>::marker);

	// fixed-vector marker
	buf.push_back(fixed ? 0x01 : 0x00);

	for (const T& it : values) {
		// values are encoded in network byte order
		// ints are encoded as U32, not U29
		T netvalue = hton(it);
		const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
		buf.insert(buf.end(), bytes, bytes + VectorProperties<T>::size);
	}

	return buf;
}

template<typename T>
AmfVector<T> AmfVector<T, typename VectorProperties<T>::type>::deserialize(
	v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != VectorProperties<T>::marker)
		throw std::invalid_argument("AmfVector: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getObject<AmfVector<T>>(type >> 1);

	unsigned int stride = VectorProperties<T>::size;
	size_t count = type >> 1;

	if (it == end)
		throw std::out_of_range("Not enough bytes for AmfVector");

	bool fixed = (*it++ == 0x01);

	if (static_cast<size_t>(end - it) < count * stride)
		throw std::out_of_range("Not enough bytes for AmfVector");

	std::vector<T> values(count);
	for (size_t i = 0; i < count; ++i) {
		// Convert value bytes to requested type.
		T val;
		std::copy(it, it + stride, reinterpret_cast<u8 *>(&val));
		it += stride;

		// Values are stored in network order.
		values[i] = ntoh(val);
	}

	AmfVector<T> ret(values, fixed);
	ctx.addObject<AmfVector<T>>(ret);

	return ret;
}

bool AmfVector<AmfItem>::operator==(const AmfItem& other) const {
	const AmfVector<AmfItem>* p = dynamic_cast<const AmfVector<AmfItem>*>(&other);
	return p != nullptr && fixed == p->fixed && type == p->type && values == p->values;
}

std::vector<u8> AmfVector<AmfItem>::serialize(SerializationContext& ctx) const {
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_VECTOR_OBJECT, u8(index << 1) };
	ctx.addObject(*this);

	// U29V value, encoding the length
	std::vector<u8> buf = AmfInteger::asLength(values.size(), AMF_VECTOR_OBJECT);

	// fixed-vector marker
	buf.push_back(fixed ? 0x01 : 0x00);

	// object type name
	std::vector<u8> typeName = AmfString(type).serializeValue(ctx);
	buf.insert(buf.end(), typeName.begin(), typeName.end());

	for (const auto& it : values) {
		auto s = it->serialize(ctx);
		buf.insert(buf.end(), s.begin(), s.end());
	}

	return buf;
}

AmfItemPtr AmfVector<AmfItem>::deserializePtr(
	v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_VECTOR_OBJECT)
		throw std::invalid_argument("AmfVector<Object>: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getPointer<AmfVector<AmfItem>>(type >> 1);

	if (it == end)
		throw std::out_of_range("Not enough bytes for AmfVector");
	bool fixed = (*it++ == 0x01);

	std::string name = AmfString::deserializeValue(it, end, ctx);
	int count = type >> 1;

	AmfItemPtr ptr(new AmfVector<AmfItem>(name, fixed));
	AmfVector<AmfItem> & vec = ptr.as<AmfVector>();
	ctx.addPointer(ptr);

	vec.values.reserve(count);
	for (int i = 0; i < count; ++i) {
		vec.values.push_back(Deserializer::deserialize(it, end, ctx));
	}

	return ptr;
}

AmfVector<AmfItem> AmfVector<AmfItem>::deserialize(
	v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	return deserializePtr(it, end, ctx).as<AmfVector<AmfItem>>();
}

template class AmfVector<int>;
template class AmfVector<unsigned int>;
template class AmfVector<double>;

} // namespace amf
