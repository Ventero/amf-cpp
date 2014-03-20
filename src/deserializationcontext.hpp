#pragma once
#ifndef DESERIALIZATIONCONTEXT_HPP
#define DESERIALIZATIONCONTEXT_HPP

#include <memory>
#include <vector>

#include "amf.hpp"

namespace amf {

class AmfItem;
class AmfObjectTraits;
class AmfString;

class DeserializationContext {
public:
	DeserializationContext() { }

	void clear();

	void addString(const AmfString& str);
	AmfString getString(int index);

	void addTraits(const AmfObjectTraits& traits);
	AmfObjectTraits getTraits(int index);

	template<typename T>
	void addObject(const T& object) {
		objects.emplace_back(new T(object));
	}

	template<typename T>
	T getObject(int index) {
		T* ptr = static_cast<T*>(objects.at(index).get());
		return T(*ptr);
	}

private:
	std::vector<std::shared_ptr<AmfString>> strings;
	std::vector<std::shared_ptr<AmfObjectTraits>> traits;
	std::vector<std::shared_ptr<AmfItem>> objects;
};

} // namespace amf

#endif
