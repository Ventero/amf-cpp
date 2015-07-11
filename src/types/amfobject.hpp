#pragma once
#ifndef AMFOBJECT_HPP
#define AMFOBJECT_HPP

#include <functional>
#include <map>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"
#include "utils/amfobjecttraits.hpp"

namespace amf {

class SerializationContext;
class DeserializationContext;

class AmfObject : public AmfItem {
public:
	AmfObject() : traits("", false, false) { }
	AmfObject(std::string className, bool dynamic, bool externalizable) :
		traits(className, dynamic, externalizable) { }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;

	template<class T>
	void addSealedProperty(std::string name, const T& value) {
		traits.attributes.insert(name);
		sealedProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	void addDynamicProperty(std::string name, const T& value) {
		dynamicProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	T& getSealedProperty(std::string name) {
		auto it = traits.attributes.find(name);
		if (it == traits.attributes.end())
			throw std::out_of_range("AmfObject::getSealedProperty");

		return sealedProperties.at(name).as<T>();
	}

	template<class T>
	T& getDynamicProperty(std::string name) {
		return dynamicProperties.at(name).as<T>();
	}

	static AmfItemPtr deserializePtr(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx);
	static AmfObject deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx);

	const AmfObjectTraits& objectTraits() const {
		return traits;
	}

	std::map<std::string, AmfItemPtr> sealedProperties;
	std::map<std::string, AmfItemPtr> dynamicProperties;

	std::function<v8(const AmfObject*)> externalizer;

private:
	AmfObject(AmfObjectTraits traits) : traits(traits) { }

	AmfObjectTraits traits;
};

} // namespace amf

#endif
