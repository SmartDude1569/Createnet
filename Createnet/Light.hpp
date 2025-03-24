#pragma once

#include <glm/glm.hpp>

#include "PhysicalInstance.hpp";

class Light : public PhysicalInstance {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
	}
};