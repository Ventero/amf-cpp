#pragma once
#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include "amf.hpp"
#include "deserializationcontext.hpp"

#include "utils/amfitemptr.hpp"

namespace amf {

class Deserializer {
public:
	Deserializer() : ctx() { }
	Deserializer(DeserializationContext ctx) : ctx(ctx) { }

	static AmfItemPtr deserialize(v8 data, DeserializationContext& ctx);
	static AmfItemPtr deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx);

	AmfItemPtr deserialize(v8 buf);
	AmfItemPtr deserialize(v8::const_iterator& it, v8::const_iterator end) {
		return deserialize(it, end, ctx);
	}

	void clearContext() { ctx.clear(); }

private:
	DeserializationContext ctx;
};

} // namespace amf

#endif
