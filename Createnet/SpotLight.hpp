#pragma once

#include <glm/glm.hpp>

#include "Light.hpp"

class SpotLight : public Light {
public:
	float constant;
	float cutOff;
	float outerCutOff;
	float linear;
	float quadratic;
	SpotLight(glm::vec3 position, glm::vec3 direction, float constant = 1.0f, glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f), float cutOff = glm::cos(glm::radians(12.5f)), float outerCutOff = glm::cos(glm::radians(15.0f)), float linear = 0.09f, float quadratic = 0.032f) : Light(ambient, diffuse, specular) {
		this->position = position;
		this->rotation = direction;
		this->constant = constant;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
		this->linear = linear;
		this->quadratic = quadratic;
	}
};