#pragma once

#include <string>
#include <iostream>

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "VirtualInstance.hpp"

class Window : public VirtualInstance {
public:
	Window(int width = 800, int height = 450, const char* title = "Createnet") {
		if (!glfwInit()) {
			std::cout << "[GLFW] couldn't initialize GLFW :/" << std::endl;
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (!window) {
			std::cout << "[GLFW] couldn't create window :/" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(0);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "[GLAD] couldn't initialize GLAD :/" << std::endl;
			glfwTerminate();
			return;
		}
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	}
	~Window() {
		glfwTerminate();
	}
	void update() {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	bool shouldClose() { return glfwWindowShouldClose(window); }
	GLFWwindow* getWindow() { return window; }
	int getWidth() {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		return width;
	}
	int getHeight() {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		return height;
	}
private:
	GLFWwindow* window;
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}
};