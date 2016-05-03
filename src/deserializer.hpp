#pragma once
#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include <functional>
#include <map>
#include <string>

#include "amf.hpp"
#include "serializationcontext.hpp"
#include "utils/amfitemptr.hpp"

namespace amf {

class AmfObject;

typedef std::function<AmfObject(v8::const_iterator&, v8::const_iterator,
	SerializationContext&)> ExternalDeserializerFunction;

class Deserializer {
public:
	Deserializer() : ctx() { }
	Deserializer(SerializationContext ctx) : ctx(ctx) { }

	AmfItemPtr deserialize(v8 buf);
	AmfItemPtr deserialize(v8::const_iterator& it, v8::const_iterator end) {
		return deserialize(it, end, ctx);
	}

	void clearContext() { ctx.clear(); }

	static AmfItemPtr deserialize(v8 data, SerializationContext& ctx);
	static AmfItemPtr deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	static std::map<std::string, ExternalDeserializerFunction> externalDeserializers;

private:
	SerializationContext ctx;
};

} // namespace amf

#endif
