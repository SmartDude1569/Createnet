#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Instance.hpp"

class PhysicalInstance : public Instance {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	PhysicalInstance() : Instance() {
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}
	bool isTransportable() override {
		return true;
	}
};