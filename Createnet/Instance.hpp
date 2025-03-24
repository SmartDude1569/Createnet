#pragma once

#include <typeinfo>

#include "Util.hpp"

class Instance {
public:
	Instance() {
		id = randomId();
	}
	virtual ~Instance() {}
	uint32_t getId() { return id; }
	virtual bool isTransportable() {
		return true;
	}
private:
	uint32_t id;
};

template <typename T>
bool isType(Instance* instance) {
	return dynamic_cast<T*>(instance) != nullptr;
};

template <typename T>
T* getAs(Instance* instance) {
	return dynamic_cast<T*>(instance);
}

std::string getClassName(Instance* instance) {
	if (!instance) return "nullptr";
	return typeid(*instance).name();
}