#pragma once
#ifndef AMFOBJECTTRAITS_HPP
#define AMFOBJECTTRAITS_HPP

#include <set>
#include <string>
#include <vector>

namespace amf {

class AmfObjectTraits {
public:
	AmfObjectTraits(std::string className, bool dynamic, bool externalizable) :
		className(className), dynamic(dynamic), externalizable(externalizable) { }

	bool operator==(const AmfObjectTraits& other) const {
		return dynamic == other.dynamic &&
			externalizable == other.externalizable &&
			className == other.className &&
			attributes == other.attributes;
	}

	bool operator!=(const AmfObjectTraits& other) const {
		return !(*this == other);
	}

	void addAttribute(std::string name) {
		if (!hasAttribute(name))
			attributes.push_back(name);
	}

	bool hasAttribute(std::string name) const {
		auto it = std::find(attributes.begin(), attributes.end(), name);
		return (it != attributes.end());
	}

	std::set<std::string> getUniqueAttributes() const {
		return std::set<std::string>(attributes.begin(), attributes.end());
	}

	std::string className;
	bool dynamic;
	bool externalizable;

	// Attribute names
	// Technically, this should be a set. However, since AMF does not actually
	// enforce the sealed property names to be unique, this needs to be a
	// vector to ensure we read the corresponding values, even if they are
	// later overwritten by another value for the same attribute name.
	// Additionally, since traits also can be sent by reference, we need to
	// actually store these duplicate names permanently, in case a later object
	// references a traits object with duplicate attribute names.
	// XXX: figure out whether this interferes with merging
	//      {de,}serializationcontext into one object.
	std::vector<std::string> attributes;

};

} // namespace amf

#endif
