#pragma once

#include "Instance.hpp"

class VirtualInstance : public Instance {
public:
	VirtualInstance() : Instance() {

	}
	bool isTransportable() override {
		return false;
	}
};