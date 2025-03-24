#pragma once

#include <glm/glm.hpp>

#include "Light.hpp"

class PointLight : public Light {
public:
	float constant;
	float linear;
	float quadratic;
	PointLight(glm::vec3 position, float constant = 1.0f, glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f), float linear = 0.09f, float quadratic = 0.032f) : Light(ambient, diffuse, specular) {
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}
};