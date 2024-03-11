#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CAM_YAW 0.0f
#define CAM_PITCH 0.0f
#define CAM_SPEED 2.5f
#define CAM_SENSITIVITY 0.01f
#define CAM_FOVY 45.0f

enum MoveDirection
{
	FORWARD,
	BACKWORD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = CAM_YAW, float pitch = CAM_PITCH);

	glm::mat4 getViewMat();
	glm::vec3 getCamPos();
	glm::vec3 getCamFront();
	float getFovY();
	void translate(MoveDirection dir, float deltaTime);
	void rotate(float offsetX, float offsetY, bool constrainPitch = true);
	void zoom(float offsetY);

private:
	void updateCameraVectors();
	glm::vec3 camPos_;
	glm::vec3 camFront_;
	glm::vec3 camUp_;
	glm::vec3 camRight_;
	glm::vec3 worldUp_;
	float fovY_;
	float mouseSensitivity_;
	float moveSpeed_;
	float yaw_;
	float pitch_;
};

