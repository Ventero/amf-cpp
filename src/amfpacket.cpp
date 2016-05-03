#include "amfpacket.hpp"

#include "deserializer.hpp"
#include "serializationcontext.hpp"
#include "types/amfnull.hpp"

namespace amf {

bool PacketHeader::operator==(const AmfItem& other) const {
	const PacketHeader* p = dynamic_cast<const PacketHeader*>(&other);
	return p != nullptr && mustUnderstand == p->mustUnderstand &&
		name == p->name && value == p->value;
}

v8 PacketHeader::serialize(SerializationContext& ctx) const {
	v8 buf;
	v8 value_data = value->serialize(ctx);
	buf.reserve(2 + name.size() + 1 + 5 + value_data.size());
	// Strings in AMF packets are always serialized as AMF0 UTF-8, i.e.
	// U16 length (in network order) U8* value
	// even though AMF3 encodes strings in a different format
	v8 name_length = network_bytes<uint16_t>(name.size());
	buf.insert(buf.end(), name_length.begin(), name_length.end());
	buf.insert(buf.end(), name.begin(), name.end());

	buf.push_back(mustUnderstand ? 0x01 : 0x00);

	// we have to mark the value as AMF3 value, which is achieved by adding
	// an AVMPLUS_OBJECT marker in front of the value. note that this counts
	// towards the value's length.
	v8 value_length = network_bytes<uint32_t>(value_data.size() + 1);
	buf.insert(buf.end(), value_length.begin(), value_length.end());
	buf.push_back(AVMPLUS_OBJECT);
	buf.insert(buf.end(), value_data.begin(), value_data.end());

	return buf;
}

PacketHeader PacketHeader::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	uint16_t name_len = read_network<uint16_t>(it, end);

	// Check for enough bytes for name and must understand flag.
	if (end - it < name_len + 1)
		throw std::out_of_range("Not enough bytes for PacketHeader");

	std::string name(it, it + name_len);
	it += name_len;

	bool mustUnderstand = (*it++ == 0x01);

	uint32_t value_len = read_network<uint32_t>(it, end);
	// If the value length is (U32)-1 the actual length is unknown, thus require
	// at least one byte for the type marker.
	if (value_len == 0xFFFFFFFF) {
		value_len = 1;
	}

	// Check that we have enough data left for the AVM object marker and value.
	// Note that the byte for the object marker is already included in value_len.
	// If value_len is invalid (i.e. 0), this check always fails.
	if (static_cast<uint32_t>(end - it) < value_len)
		throw std::out_of_range("Not enough bytes for PacketHeader");

	if (*it++ != AVMPLUS_OBJECT)
		throw std::invalid_argument("PacketHeader: Invalid type marker");

	PacketHeader header(name, mustUnderstand, AmfNull());
	header.value = Deserializer::deserialize(it, end, ctx);

	return header;
}

bool PacketMessage::operator==(const AmfItem& other) const {
	const PacketMessage* p = dynamic_cast<const PacketMessage*>(&other);
	return p != nullptr && target == p->target && response == p->response &&
		value == p->value;
}

v8 PacketMessage::serialize(SerializationContext& ctx) const {
	v8 buf;
	v8 value_data = value->serialize(ctx);
	buf.reserve(2 + target.size() + 2 + response.size() + 5 + value_data.size());

	v8 target_length = network_bytes<uint16_t>(target.size());
	buf.insert(buf.end(), target_length.begin(), target_length.end());
	buf.insert(buf.end(), target.begin(), target.end());

	v8 response_length = network_bytes<uint16_t>(response.size());
	buf.insert(buf.end(), response_length.begin(), response_length.end());
	buf.insert(buf.end(), response.begin(), response.end());

	v8 value_length = network_bytes<uint32_t>(value_data.size() + 1);
	buf.insert(buf.end(), value_length.begin(), value_length.end());
	buf.push_back(AVMPLUS_OBJECT);
	buf.insert(buf.end(), value_data.begin(), value_data.end());

	return buf;
}

PacketMessage PacketMessage::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	uint16_t target_len = read_network<uint16_t>(it, end);
	if (end - it < target_len)
		throw std::out_of_range("Not enough bytes for PacketMessage");

	std::string target(it, it + target_len);
	it += target_len;

	uint16_t response_len = read_network<uint16_t>(it, end);
	if (end - it < response_len)
		throw std::out_of_range("Not enough bytes for PacketMessage");

	std::string response(it, it + response_len);
	it += response_len;

	uint32_t value_len = read_network<uint32_t>(it, end);
	// If the value length is (U32)-1 the actual length is unknown, thus require
	// at least one byte for the type marker.
	if (value_len == 0xFFFFFFFF) {
		value_len = 1;
	}

	// Check that we have enough data left for the AVM object marker and value.
	// Note that the byte for the object marker is already included in value_len.
	// If value_len is invalid (i.e. 0), this check always fails.
	if (static_cast<uint32_t>(end - it) < value_len)
		throw std::out_of_range("Not enough bytes for PacketMessage");

	if (*it++ != AVMPLUS_OBJECT)
		throw std::invalid_argument("PacketMessage: Invalid type marker");

	PacketMessage message(target, response, AmfNull());
	message.value = Deserializer::deserialize(it, end, ctx);

	return message;
}

bool AmfPacket::operator==(const AmfItem& other) const {
	const AmfPacket* p = dynamic_cast<const AmfPacket*>(&other);
	return p != nullptr && headers == p->headers && messages == p->messages;
}

v8 AmfPacket::serialize(SerializationContext& ctx) const {
	if (headers.size() >= 65536)
		throw std::length_error("AmfPacket::serialize too many headers");

	if (messages.size() >= 65536)
		throw std::length_error("AmfPacket::serialize too many messages");

	v8 buf = {
		0x00, 0x03 // Version is always AMF3
	};

	v8 header_count = network_bytes<uint16_t>(headers.size());
	buf.insert(buf.end(), header_count.begin(), header_count.end());
	for (const PacketHeader& header : headers) {
		const v8& val = header.serialize(ctx);
		buf.insert(buf.end(), val.begin(), val.end());
	}

	v8 message_count = network_bytes<uint16_t>(messages.size());
	buf.insert(buf.end(), message_count.begin(), message_count.end());
	for (const PacketMessage& message : messages) {
		const v8& val = message.serialize(ctx);
		buf.insert(buf.end(), val.begin(), val.end());
	}

	return buf;
}

AmfPacket AmfPacket::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	// 2 bytes required for version, header count and message count each.
	if (end - it < 2 + 2 + 2)
		throw std::out_of_range("Not enough bytes for AmfPacket");

	if (*it++ != 0x00 || *it++ != 0x03)
		throw std::invalid_argument("AmfPacket: Invalid type marker");

	AmfPacket p;

	uint16_t headers = read_network<uint16_t>(it, end);
	p.headers.reserve(headers);
	for (int h = 0; h < headers; ++h) {
		p.headers.push_back(PacketHeader::deserialize(it, end, ctx));
	}

	uint16_t messages = read_network<uint16_t>(it, end);
	p.messages.reserve(messages);
	for (int m = 0; m < messages; ++m) {
		p.messages.push_back(PacketMessage::deserialize(it, end, ctx));
	}

	return p;
}


} // namespace amf
