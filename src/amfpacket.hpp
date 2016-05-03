#pragma once
#ifndef AMFPACKET_HPP
#define AMFPACKET_HPP

#include <string>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"

namespace amf {

class SerializationContext;

enum Amf0Marker : u8 {
	AVMPLUS_OBJECT = 0x11
};

class PacketHeader : public AmfItem {
public:
	template<typename T>
	PacketHeader(std::string name, bool mustUnderstand, const T& value) :
		name(name), mustUnderstand(mustUnderstand), value(new T(value)) { }

	bool operator==(const AmfItem& other) const;
	v8 serialize(SerializationContext& ctx) const;
	static PacketHeader deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	template<typename T>
	T& getValue() {
		return value.as<T>();
	}

	std::string name;
	bool mustUnderstand;

private:
	AmfItemPtr value;
};

class PacketMessage : public AmfItem {
public:
	template<typename T>
	PacketMessage(std::string targetUri, std::string responseUri, const T& value) :
		target(targetUri), response(responseUri), value(new T(value)) { }

	bool operator==(const AmfItem& other) const;
	v8 serialize(SerializationContext& ctx) const;
	static PacketMessage deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	template<typename T>
	T& getValue() {
		return value.as<T>();
	}

	std::string target;
	std::string response;

private:
	AmfItemPtr value;
};

class AmfPacket : public AmfItem {
public:
	AmfPacket() { }

	bool operator==(const AmfItem& other) const;
	v8 serialize(SerializationContext& ctx) const;
	static AmfPacket deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::vector<PacketHeader> headers;
	std::vector<PacketMessage> messages;
};

}

#endif
