#pragma once
#ifndef AMFOBJECT_HPP
#define AMFOBJECT_HPP

#include <functional>
#include <map>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"
#include "utils/amfobjecttraits.hpp"

namespace amf {

class DeserializationContext;

class AmfObject : public AmfItem {
public:
	AmfObject() : traits("", false, false) { }
	AmfObject(std::string className, bool dynamic, bool externalizable) :
		traits(AmfObjectTraits(className, dynamic, externalizable)) { }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize() const;

	template<class T>
	void addSealedProperty(std::string name, const T& value) {
		auto* a = &traits.attributes;
		if (std::find(a->begin(), a->end(), name) == a->end())
			traits.attributes.push_back(name);

		sealedProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	void addDynamicProperty(std::string name, const T& value) {
		dynamicProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	T& getSealedProperty(std::string name) {
		const auto& it = std::find(traits.attributes.begin(),
			traits.attributes.end(), name);

		if (it == traits.attributes.end())
			throw std::out_of_range("AmfObject::getSealedProperty");

		return sealedProperties.at(name).as<T>();
	}

	template<class T>
	T& getDynamicProperty(std::string name) {
		return dynamicProperties.at(name).as<T>();
	}

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
