#include "amfdate.hpp"

#include "serializationcontext.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"

namespace amf {

AmfDate::AmfDate(std::chrono::system_clock::time_point date) {
	auto duration = date.time_since_epoch();
	value = std::chrono::duration_cast<std::chrono::milliseconds>(
		duration).count();
}

bool AmfDate::operator==(const AmfItem& other) const {
	const AmfDate* p = dynamic_cast<const AmfDate*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfDate::serialize(SerializationContext& ctx) const {
	// AmfDate is date-marker (U29O-ref | (U29D-value date-time)),
	// where U29D-value is 1 and date-time is a int64 describing the number of
	// milliseconds since epoch, encoded as double
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_DATE, u8(index << 1) };
	ctx.addObject(*this);

	std::vector<u8> buf { AMF_DATE, 0x01 };

	// dates are serialised as double, ignoring the precision loss
	std::vector<u8> date(AmfDouble(static_cast<double>(value)).serialize(ctx));
	// skip the AMF_DOUBLE marker
	buf.insert(buf.end(), date.begin() + 1, date.end());

	return buf;
}

AmfDate AmfDate::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_DATE)
		throw std::invalid_argument("AmfDate: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getObject<AmfDate>(type >> 1);

	if (end - it < 8)
		throw std::out_of_range("Not enough bytes for AmfDate");

	double v;
	std::copy(it, it + 8, reinterpret_cast<u8 *>(&v));
	it += 8;

	AmfDate ret(static_cast<long long>(ntoh(v)));
	ctx.addObject<AmfDate>(ret);

	return ret;
}

} // namespace amf
