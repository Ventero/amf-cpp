#include "serializationcontext.hpp"

namespace amf {

void SerializationContext::clear() {
	strings.clear();
	traits.clear();
	objects.clear();
}

int SerializationContext::getIndex(const std::string& str) const {
	auto it = std::find(strings.begin(), strings.end(), str);
	if (it == strings.end())
		return -1;

	return it - strings.begin();
}

int SerializationContext::getIndex(const AmfObjectTraits& str) const {
	auto it = std::find(traits.begin(), traits.end(), str);
	if (it == traits.end())
		return -1;

	return it - traits.begin();
}

}
