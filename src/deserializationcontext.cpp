#include "deserializationcontext.hpp"

#include "types/amfobject.hpp"
#include "types/amfstring.hpp"

namespace amf {

void DeserializationContext::clear() {
	strings.clear();
	traits.clear();
	objects.clear();
}

void DeserializationContext::addString(const AmfString& str) {
	if (str.value.emtpy()) return;

	strings.emplace_back(new AmfString(str));
}

AmfString DeserializationContext::getString(int index) {
	return AmfString(*strings.at(index));
}

void DeserializationContext::addTraits(const AmfObjectTraits& trait) {
	traits.emplace_back(new AmfObjectTraits(trait));
}

AmfObjectTraits DeserializationContext::getTraits(int index) {
	return AmfObjectTraits(*traits.at(index));
}

} // namespace amf
