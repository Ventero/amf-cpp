#include "deserializationcontext.hpp"

#include "types/amfobject.hpp"
#include "types/amfstring.hpp"

namespace amf {

void DeserializationContext::clear() {
	strings.clear();
	traits.clear();
	objects.clear();
}

void DeserializationContext::addString(const std::string& str) {
	if (str.empty()) return;

	strings.push_back(str);
}

AmfString DeserializationContext::getString(int index) {
	return strings.at(index);
}

void DeserializationContext::addTraits(const AmfObjectTraits& trait) {
	traits.emplace_back(new AmfObjectTraits(trait));
}

AmfObjectTraits DeserializationContext::getTraits(int index) {
	return AmfObjectTraits(*traits.at(index));
}

} // namespace amf
