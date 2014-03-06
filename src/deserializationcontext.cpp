#include "deserializationcontext.hpp"

#include "types/amfbytearray.hpp"
#include "types/amfdate.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"

namespace amf {

void DeserializationContext::clear() {
	strings.clear();
	traits.clear();
	objects.clear();
}

void DeserializationContext::addString(const AmfString& str) {
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

template<typename T>
void DeserializationContext::addObject(const T& object) {
	objects.emplace_back(new T(object));
}

template<typename T>
T DeserializationContext::getObject(int index) {
	T* ptr = static_cast<T*>(objects.at(index).get());
	return T(*ptr);
}

// explicit instantiations
template void DeserializationContext::addObject<AmfByteArray>(const AmfByteArray&);
template AmfByteArray DeserializationContext::getObject<AmfByteArray>(int);

template void DeserializationContext::addObject<AmfDate>(const AmfDate&);
template AmfDate DeserializationContext::getObject<AmfDate>(int);
} // namespace amf
