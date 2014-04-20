#pragma once
#ifndef DESERIALIZATIONCONTEXT_HPP
#define DESERIALIZATIONCONTEXT_HPP

#include <memory>
#include <vector>

#include "amf.hpp"

#include "utils/amfitemptr.hpp"

namespace amf {

class AmfObjectTraits;

class DeserializationContext {
public:
	DeserializationContext() { }

	void clear();

	void addString(const std::string& str);
	std::string getString(size_t index);

	void addTraits(const AmfObjectTraits& traits);
	AmfObjectTraits getTraits(size_t index);

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
	T getObject(size_t index) {
		T* ptr = dynamic_cast<T*>(objects.at(index).get());
		if (ptr == nullptr)
			throw std::invalid_argument("DeserializationContext::getObject wrong type");

		return T(*ptr);
	}

private:
	std::vector<std::string> strings;
	std::vector<std::shared_ptr<AmfObjectTraits>> traits;
	std::vector<AmfItemPtr> objects;
};

} // namespace amf

#endif
