#pragma once
#ifndef AMFXML_HPP
#define AMFXML_HPP

#include <string>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

namespace amf {

class AmfXml : public AmfItem {
public:
	AmfXml() { };
	AmfXml(std::string value) : value(value) { };

	bool operator==(const AmfItem& other) const {
		const AmfXml* p = dynamic_cast<const AmfXml*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(value.size()).asLength(AMF_XML);

		// the actual data is simply encoded as UTF8-chars
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

	static AmfXml deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		int type = AmfInteger::deserialize(it, end, ctx).value;
		if ((type & 0x01) == 0)
			return ctx.getObject<AmfXml>(type >> 1);

		int length = type >> 1;
		if (end - it < length)
			throw std::out_of_range("Not enough bytes for AmfXml");

		std::string val(it, it + length);
		it += length;

		AmfXml ret(val);
		ctx.addObject<AmfXml>(ret);

		return ret;
	}

	std::string value;
};

} // namespace amf

#endif
