#pragma once
#ifndef AMFPACKET_HPP
#define AMFPACKET_HPP

#include "types/amfitem.hpp"

#include <cassert>

namespace amf {

const u8 AVMPLUS_OBJECT_MARKER = 0x11;

class PacketHeader : public AmfItem {
public:
	PacketHeader(std::string name, bool mustUnderstand, const AmfItem& value) :
		name(name), mustUnderstand(mustUnderstand), value(value.serialize()) { };

	v8 serialize() const {
		v8 buf;
		buf.reserve(2 + name.size() + 1 + 5 + value.size());
		// Strings in AMF packets are always serialized as AMF0 UTF-8, i.e.
		// U16 length (in network order) U8* value
		// even though AMF3 encodes strings in a different format
		v8 name_length = network_bytes<uint16_t>(name.size());
		buf.insert(buf.end(), name_length.begin(), name_length.end());
		buf.insert(buf.end(), name.begin(), name.end());

		buf.push_back(mustUnderstand ? 0x01 : 0x00);

		// we have to mark the value as AMF3 value, which is achieved by adding
		// an AVMPLUS_OBJECT_MARKER in front of the value. note that this counts
		// towards the value's length.
		v8 value_length = network_bytes<uint32_t>(value.size() + 1);
		buf.insert(buf.end(), value_length.begin(), value_length.end());
		buf.push_back(AVMPLUS_OBJECT_MARKER);
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string name;
	bool mustUnderstand;
	v8 value;
};

class PacketMessage : public AmfItem {
public:
	PacketMessage(std::string targetUri, std::string responseUri, const AmfItem& value) :
		target(targetUri), response(responseUri), value(value.serialize()) { };

	v8 serialize() const {
		v8 buf;
		buf.reserve(2 + target.size() + 2 + response.size() + 5 + value.size());

		v8 target_length = network_bytes<uint16_t>(target.size());
		buf.insert(buf.end(), target_length.begin(), target_length.end());
		buf.insert(buf.end(), target.begin(), target.end());

		v8 response_length = network_bytes<uint16_t>(response.size());
		buf.insert(buf.end(), response_length.begin(), response_length.end());
		buf.insert(buf.end(), response.begin(), response.end());

		v8 value_length = network_bytes<uint32_t>(value.size() + 1);
		buf.insert(buf.end(), value_length.begin(), value_length.end());
		buf.push_back(AVMPLUS_OBJECT_MARKER);
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string target;
	std::string response;
	v8 value;
};

class AmfPacket {
public:
	AmfPacket() { };

	v8 serialize() const {
		assert(headers.size() < 65536 && "Too many headers");
		assert(messages.size() < 65536 && "Too many messages");

		v8 buf = {
			0x00, 0x03 // Version is always AMF3
		};

		v8 header_count = network_bytes<uint16_t>(headers.size());
		buf.insert(buf.end(), header_count.begin(), header_count.end());
		for(const PacketHeader& header : headers) {
			const v8& val = header.serialize();
			buf.insert(buf.end(), val.begin(), val.end());
		}

		v8 message_count = network_bytes<uint16_t>(messages.size());
		buf.insert(buf.end(), message_count.begin(), message_count.end());
		for(const PacketMessage& message : messages) {
			const v8& val = message.serialize();
			buf.insert(buf.end(), val.begin(), val.end());
		}

		return buf;
	}

	std::vector<PacketHeader> headers;
	std::vector<PacketMessage> messages;
};

}

#endif
