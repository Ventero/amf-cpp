#include "serializationcontext.hpp"

namespace amf {

void SerializationContext::clear() {
	strings.clear();
	traits.clear();
	objects.clear();
}

}
