#pragma once
#ifndef AMFOBJECTTRAITS_HPP
#define AMFOBJECTTRAITS_HPP

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

	void addAttribute(const std::string & attr) {
		if (!isAttributeExists(attr)) {
			attributes.push_back(attr);
		}
	}
	
	const std::vector<std::string> & getAttriutes() const {
		return attributes;
	}
	bool isAttributeExists(const std::string & attr) {
		return std::find(attributes.begin(), attributes.end(), attr) != attributes.end();
	}
	std::string className;
	
	bool dynamic;
	bool externalizable;
private:
	std::vector<std::string> attributes;

};

} // namespace amf

#endif
