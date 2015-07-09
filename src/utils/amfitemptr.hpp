#pragma once
#ifndef AMFITEMPTR_HPP
#define AMFITEMPTR_HPP

#include <memory>

#include "types/amfitem.hpp"

namespace amf {

class AmfItemPtr : private std::shared_ptr<AmfItem> {
public:
	explicit AmfItemPtr() : std::shared_ptr<AmfItem>() { }
	explicit AmfItemPtr(AmfItem* ptr) : std::shared_ptr<AmfItem>(ptr) { }

	template<typename T, typename std::enable_if<std::is_base_of<AmfItem, T>::value, int>::type = 0>
	explicit AmfItemPtr(const T& ref) : std::shared_ptr<AmfItem>(new T(ref)) { }

	template<typename T, typename std::enable_if<std::is_base_of<AmfItem, T>::value, int>::type = 0>
	T& as() {
		return dynamic_cast<T&>(*get());;
	}

	template<typename T, typename std::enable_if<std::is_base_of<AmfItem, T>::value, int>::type = 0>
	const T& as() const {
		return dynamic_cast<const T&>(*get());
	}

	// WARNING: the pointer returned by this and get() is only valid as long as
	//          the AmfItemPtr is still alive
	template<typename T, typename std::enable_if<std::is_base_of<AmfItem, T>::value, int>::type = 0>
	T* asPtr() {
		return dynamic_cast<T*>(get());
	}

	template<typename T, typename std::enable_if<std::is_base_of<AmfItem, T>::value, int>::type = 0>
	const T* asPtr() const {
		return dynamic_cast<const T*>(get());
	}

	bool operator==(const AmfItemPtr& other) const {
		return this->get() == other.get() || *this->get() == *other.get();
	}

	bool operator!=(const AmfItemPtr& other) const {
		return !(*this == other);
	}

	using std::shared_ptr<AmfItem>::get;
	using std::shared_ptr<AmfItem>::reset;
	using std::shared_ptr<AmfItem>::operator*;
	using std::shared_ptr<AmfItem>::operator->;
};

}

#endif
