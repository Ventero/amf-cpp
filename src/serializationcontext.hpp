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

	void clear();

	void addString(const std::string& str) {
		if (str.empty()) return;

		strings.push_back(str);
	}

	const std::string & getString(size_t index) const {
		return strings.at(index);
	}

	void addTraits(const AmfObjectTraits& trait) {
		traits.push_back(trait);
	}

	const AmfObjectTraits & getTraits(size_t index) const {
		return traits.at(index);
	}

	template<typename T>
	void addObject(const T & obj) {
		objects.emplace_back(new T(obj));
	}

	template<typename T>
	const T & getObject(size_t index) const {
		AmfItemPtr ptr = getPointer<T>(index);
		return ptr.as<T>();
	}

	void addPointer(const AmfItemPtr & ptr) {
		objects.push_back(ptr);
	}

	template<typename T>
	const AmfItemPtr & getPointer(size_t index) const {
		const AmfItemPtr & ptr = objects.at(index);

		if (ptr.asPtr<T>() == nullptr)
			throw std::invalid_argument("SerializationContext::getPointer wrong type");

		return ptr;
	}

	int getIndex(const std::string& str) const;

	int getIndex(const AmfObjectTraits& str) const;

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
