#pragma once
#ifndef DESERIALIZATIONCONTEXT_HPP
#define DESERIALIZATIONCONTEXT_HPP

#include <vector>

#include "amf.hpp"
#include "utils/amfitemptr.hpp"
#include "utils/amfobjecttraits.hpp"

namespace amf {

class DeserializationContext {
public:
	DeserializationContext() { }

	void clear();

	void addString(const std::string& str);
	const std::string & getString(size_t index);

	void addTraits(const AmfObjectTraits& traits);
	const AmfObjectTraits & getTraits(size_t index);

	size_t addPointer(const AmfItemPtr & ptr) {
		objects.push_back(ptr);
		return objects.size() - 1;
	}

	template<typename T>
	AmfItemPtr getPointer(size_t index) {
		const AmfItemPtr & ptr = objects.at(index);

		if (ptr.asPtr<T>() == nullptr)
			throw std::invalid_argument("DeserializationContext::getPointer wrong type");

		return ptr;
	}

	template<typename T>
	size_t addObject(const T& object) {
		objects.emplace_back(new T(object));
		return objects.size() - 1;
	}

	template<typename T>
	void setObject(size_t index, const T& object) {
		objects.at(index) = AmfItemPtr(new T(object));
	}

	template<typename T>
	const T & getObject(size_t index) {
		AmfItemPtr ptr = getPointer<T>(index);
		return ptr.as<T>();
	}

private:
	std::vector<std::string> strings;
	std::vector<AmfObjectTraits> traits;
	std::vector<AmfItemPtr> objects;
};

} // namespace amf

#endif
