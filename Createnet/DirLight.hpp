#pragma once

#include <glm/glm.hpp>

#include "Light.hpp"

class DirLight : public Light {
public:
	DirLight(glm::vec3 direction, glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f)) : Light(ambient, diffuse, specular) {
		this->rotation = direction;
	}
};