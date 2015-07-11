#pragma once
#ifndef SERIALIZATIONCONTEXT_HPP
#define SERIALIZATIONCONTEXT_HPP

#include <vector>

#include "amf.hpp"
#include "utils/amfitemptr.hpp"
#include "utils/amfobjecttraits.hpp"

namespace amf {

class SerializationContext {
public:
	SerializationContext() { }

	void clear() {
		strings.clear();
		traits.clear();
		objects.clear();
	}

	void addString(const std::string& str) {
		strings.push_back(str);
	}

	void addTraits(const AmfObjectTraits& trait) {
		traits.push_back(trait);
	}

	template<typename T>
	void addObject(const T & obj) {
		objects.emplace_back(new T(obj));
	}

	int getIndex(const std::string& str) {
		auto it = std::find(strings.begin(), strings.end(), str);
		if (it == strings.end())
			return -1;

		return it - strings.begin();
	}

	int getIndex(const AmfObjectTraits& str) {
		auto it = std::find(traits.begin(), traits.end(), str);
		if (it == traits.end())
			return -1;

		return it - traits.begin();
	}

	template<typename T>
	int getIndex(const T & obj) const {
		for (size_t i = 0; i < objects.size(); ++i) {
			const T* typeval = objects[i].asPtr<T>();

			if (typeval != nullptr && *typeval == obj)
				return static_cast<int>(i);
		}

		return -1;
	}

private:
	std::vector<std::string> strings;
	std::vector<AmfObjectTraits> traits;
	std::vector<AmfItemPtr> objects;
};

} // namespace amf

#endif
