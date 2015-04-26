#include "deserializationcontext.hpp"

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

const std::string & DeserializationContext::getString(size_t index) {
	return strings.at(index);
}

void DeserializationContext::addTraits(const AmfObjectTraits& trait) {
	traits.push_back(trait);
}

const AmfObjectTraits & DeserializationContext::getTraits(size_t index) {
	return traits.at(index);
}

} // namespace amf
