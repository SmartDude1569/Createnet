#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../glad/glad.h"
#include <glm/glm.hpp>

#include "Instance.hpp"
#include "VirtualInstance.hpp"
#include "Light.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "DirLight.hpp"

class Shader : public VirtualInstance {
public:
	Shader(const char* vertexCode, const char* fragmentCode) {
		unsigned int vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		const char* vCode = vertexCode;
		const char* fCode = fragmentCode;
		glShaderSource(vertex, 1, &vCode, NULL);
		glShaderSource(fragment, 1, &fCode, NULL);
		glCompileShader(vertex);
		glCompileShader(fragment);
		checkCompileErrors(vertex, "VERTEX");
		checkCompileErrors(fragment, "FRAGMENT");
		shaderId = glCreateProgram();
		glAttachShader(shaderId, vertex);
		glAttachShader(shaderId, fragment);
		glLinkProgram(shaderId);
		checkCompileErrors(shaderId, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void use() {
		glUseProgram(shaderId);
	}
	void setTransformations(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
		setMat4("model", model);
		setMat4("view", view);
		setMat4("projection", projection);
	}
	void addLight(Light* light) {
		if (isType<PointLight>(light)) {
			PointLight* pointLight = dynamic_cast<PointLight*>(light);
			addPointLight(pointLight, pointLightIdx++);
		}
		else if (isType<SpotLight>(light)) {
			SpotLight* spotLight = dynamic_cast<SpotLight*>(light);
			addSpotLight(spotLight, spotLightIdx++);
		}
		else if (isType<DirLight>(light)) {
			DirLight* dirLight = dynamic_cast<DirLight*>(light);
			addDirLight(dirLight, dirLightIdx++);
		}
	}
	void resetLightIndices() {
		pointLightIdx = 0;
		spotLightIdx = 0;
		dirLightIdx = 0;
	}
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
	}
	void setVec2(const std::string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(shaderId, name.c_str()), x, y);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(shaderId, name.c_str()), x, y, z);
	}
	void setVec4(const std::string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(shaderId, name.c_str()), x, y, z, w);
	}
	void setMat2(const std::string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	unsigned int pointLightIdx;
	unsigned int spotLightIdx;
	unsigned int dirLightIdx;
	unsigned int shaderId;
	void addPointLight(const PointLight* light, int index) {
		setVec3("pointLights[" + std::to_string(index) + "].position", light->position);
		setVec3("pointLights[" + std::to_string(index) + "].ambient", light->ambient);
		setVec3("pointLights[" + std::to_string(index) + "].diffuse", light->diffuse);
		setVec3("pointLights[" + std::to_string(index) + "].specular", light->specular);
		setFloat("pointLights[" + std::to_string(index) + "].constant", light->constant);
		setFloat("pointLights[" + std::to_string(index) + "].linear", light->linear);
		setFloat("pointLights[" + std::to_string(index) + "].quadratic", light->quadratic);
	}
	void addSpotLight(const SpotLight* light, int index) {
		setVec3("spotLights[" + std::to_string(index) + "].position", light->position);
		setVec3("spotLights[" + std::to_string(index) + "].direction", light->rotation);
		setVec3("spotLights[" + std::to_string(index) + "].ambient", light->ambient);
		setVec3("spotLights[" + std::to_string(index) + "].diffuse", light->diffuse);
		setVec3("spotLights[" + std::to_string(index) + "].specular", light->specular);
		setFloat("spotLights[" + std::to_string(index) + "].constant", light->constant);
		setFloat("spotLights[" + std::to_string(index) + "].linear", light->linear);
		setFloat("spotLights[" + std::to_string(index) + "].quadratic", light->quadratic);
		setFloat("spotLights[" + std::to_string(index) + "].cutOff", light->cutOff);
		setFloat("spotLights[" + std::to_string(index) + "].outerCutOff", light->outerCutOff);
	}
	void addDirLight(const DirLight* light, int index) {
		setVec3("dirLights[" + std::to_string(index) + "].direction", light->rotation);
		setVec3("dirLights[" + std::to_string(index) + "].ambient", light->ambient);
		setVec3("dirLights[" + std::to_string(index) + "].diffuse", light->diffuse);
		setVec3("dirLights[" + std::to_string(index) + "].specular", light->specular);
	}
	void checkCompileErrors(GLuint shader, std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "[SHADER] compilation error of type \"" << type << "\": " << infoLog << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "[SHADER] linking error of type \"" << type << "\": " << infoLog << std::endl;
			}
		}
	}
};