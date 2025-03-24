#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "Shader.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Instance.hpp"
#include "PhysicalInstance.hpp"
#include "VirtualInstance.hpp"

class Client : public VirtualInstance {
public:
	Client() {
		window = new Window();
		camera = new Camera(glm::vec3(0, 0, 0));
		std::string vertexShader = getResource("vertMain.glsl", "https://smartdude.dev/vertMain.glsl");
		std::string fragmentShader = getResource("fragMain.glsl", "https://smartdude.dev/fragMain.glsl");
		mainShader = new Shader(vertexShader.c_str(), fragmentShader.c_str());
		Model sponza = Model("./sponza/sponza.obj");
		sponza.position = glm::vec3(0, 0, 0);
		sponza.scale = glm::vec3(0.05f, 0.05f, 0.05f);
		instances.insert_or_assign(sponza.getId(), std::make_unique<Model>(sponza));
		DirLight light = DirLight(glm::vec3(0.2f, -1.0f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
		instances.insert_or_assign(light.getId(), std::make_unique<DirLight>(light));
		glfwSetWindowUserPointer(window->getWindow(), this);
		glfwSetCursorPosCallback(window->getWindow(), Client::mouseCallback);
		glfwSetMouseButtonCallback(window->getWindow(), Client::mouseClickCallback);
		this->focused = false;
	}
	void loop() {
		float currentFrame = glfwGetTime();
		processKeyboard();
		window->update();
		render();
		dt = glfwGetTime() - currentFrame;
		fps = 1.0f / dt;
	}
	void render() {
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		preRender();
		for (auto& pair : instances) {
			uint32_t id = pair.first;
			Instance* instance = pair.second.get();
			if (isType<Light>(instance)) {
				mainShader->addLight(dynamic_cast<Light*>(instance));
			}
			if (isType<Mesh>(instance)) {
				renderMesh(dynamic_cast<Mesh*>(instance));
			}
			if (isType<Model>(instance)) {
				renderModel(dynamic_cast<Model*>(instance));
			}
		}
	}
	void processKeyboard() {
		GLFWwindow* window = this->window->getWindow();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->processCameraMovement(FORWARD, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->processCameraMovement(BACKWARD, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->processCameraMovement(LEFT, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->processCameraMovement(RIGHT, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera->processCameraMovement(UP, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera->processCameraMovement(DOWN, dt);
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			this->focused = false;
		}
	}
	void processMouse(double x, double y) {
		static float lastX = 400, lastY = 300;
		static bool firstMouse = true;
		if (firstMouse) {
			lastX = x;
			lastY = y;
			firstMouse = false;
		}
		float xOffset = x - lastX;
		float yOffset = lastY - y;
		lastX = x;
		lastY = y;
		camera->processCameraTurn(xOffset, yOffset);
	}
	bool getFocused() {
		return this->focused;
	}
	void setFocused(bool focused) {
		this->focused = focused;
	}
	bool shouldClose() {
		return window->shouldClose();
	}
private:
	Window* window;
	Camera* camera;
	Shader* mainShader;
	std::unordered_map<uint32_t, std::unique_ptr<Instance>> instances;
	float dt;
	float fps;
	bool focused;
	static void mouseCallback(GLFWwindow* window, double x, double y) {
		Client* client = (Client*)glfwGetWindowUserPointer(window);
		if (client->getFocused()) client->processMouse(x, y);
	}
	static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
		Client* client = (Client*)glfwGetWindowUserPointer(window);
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !client->getFocused()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			client->setFocused(true);
		}
	}
	void preRender() {
		mainShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);
		glm::mat4 view = camera->getViewMatrix();
		mainShader->setMat4("projection", projection);
		mainShader->setMat4("view", view);
		mainShader->setVec3("viewPos", camera->position);
		mainShader->setBool("shading", true);
		mainShader->setFloat("shininess", 4.0f);
		mainShader->resetLightIndices();
	}
	void applyModelMatrix(PhysicalInstance* instance, Shader* shader) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, instance->position);
		model = glm::rotate(model, glm::radians(instance->rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(instance->rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(instance->rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, instance->scale);
		shader->setMat4("model", model);
	}
	void renderMesh(Mesh* mesh) {
		applyModelMatrix(mesh, mainShader);
		mesh->draw(*mainShader);
	}
	void renderModel(Model* model) {
		applyModelMatrix(model, mainShader);
		model->draw(*mainShader);
	}
};