#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

const float INITIAL_YAW = -90.0f;
const float INITIAL_PITCH = 0.0f;
const float CAMERA_SPEED = 2.5f;
const float CAMERA_SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera : public VirtualInstance {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	float yaw;
	float pitch;
	float movementSpeed;
	float mouseSensitivity;
	float zoom;
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = INITIAL_YAW, float pitch = INITIAL_PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(CAMERA_SPEED), mouseSensitivity(CAMERA_SENSITIVITY), zoom(ZOOM) {
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(CAMERA_SPEED), mouseSensitivity(CAMERA_SENSITIVITY), zoom(ZOOM) {
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}
	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}
	void processCameraMovement(CameraMovement direction, float dt) {
		float vel = movementSpeed * dt;
		if (direction == FORWARD) position += front * vel;
		if (direction == BACKWARD) position -= front * vel;
		if (direction == LEFT) position -= right * vel;
		if (direction == RIGHT) position += right * vel;
		if (direction == UP) position += up * vel;
		if (direction == DOWN) position -= up * vel;
	}
	void processCameraTurn(float xOffset, float yOffset, bool constrainPitch = true) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;
		yaw += xOffset;
		pitch += yOffset;
		if (constrainPitch) {
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;
		}
		updateCameraVectors();
	}
	void processCameraZoom(float yOffset) {
		zoom -= yOffset;
		if (zoom < 1.0f) zoom = 1.0f;
		if (zoom > 60.0f) zoom = 60.0f;
	}
private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};