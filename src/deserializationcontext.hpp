#pragma once
#ifndef DESERIALIZATIONCONTEXT_HPP
#define DESERIALIZATIONCONTEXT_HPP

#include <memory>
#include <vector>

#include "amf.hpp"

#include "utils/amfitemptr.hpp"

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
	int addObject(const T& object) {
		objects.emplace_back(new T(object));
		return objects.size() - 1;
	}

	template<typename T>
	void setObject(int index, const T& object) {
		objects.at(index) = AmfItemPtr(new T(object));
	}

	void addObjectPtr(AmfItemPtr object) {
		objects.push_back(object);
	}

	template<typename T>
	T getObject(int index) {
		T* ptr = static_cast<T*>(objects.at(index).get());
		return T(*ptr);
	}

	const AmfItemPtr& getObjectPtr(int index) const {
		return objects.at(index);
	}

private:
	std::vector<std::shared_ptr<AmfString>> strings;
	std::vector<std::shared_ptr<AmfObjectTraits>> traits;
	std::vector<AmfItemPtr> objects;
};

} // namespace amf

#endif
