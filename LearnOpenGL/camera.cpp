#include "camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
	: camFront_(glm::vec3(0.0f, 0.0f, -1.0f)),
	  moveSpeed_(CAM_SPEED),
	  mouseSensitivity_(CAM_SENSITIVITY),
	  fovY_(CAM_FOVY)
{
	camPos_ = pos;
	worldUp_ = up;
	yaw_ = yaw;
	pitch_ = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMat()
{
	return glm::lookAt(camPos_, camPos_ + camFront_, camUp_);
}

glm::vec3 Camera::getCamPos()
{
	return camPos_;
}

float Camera::getFovY()
{
	return fovY_;
}

void Camera::translate(MoveDirection dir, float deltaTime)
{
	switch (dir)
	{
	case FORWARD:
		camPos_ += camFront_ * moveSpeed_ * deltaTime;
		break;
	case BACKWORD:
		camPos_ -= camFront_ * moveSpeed_ * deltaTime;
		break;
	case LEFT:
		camPos_ -= camRight_ * moveSpeed_ * deltaTime;
		break;
	case RIGHT:
		camPos_ += camRight_ * moveSpeed_ * deltaTime;
		break;
	case UP:
		camPos_ += worldUp_ * moveSpeed_ * deltaTime;
		break;
	case DOWN:
		camPos_ -= worldUp_ * moveSpeed_ * deltaTime;
		break;
	default:
		break;
	}
}

void Camera::zoom(float offsetY)
{
	fovY_ -= (float)offsetY;
	if (fovY_ < 1.0f)
		fovY_ = 1.0f;
	if (fovY_ > 45.0f)
		fovY_ = 45.0f;
}

void Camera::rotate(float offsetX, float offsetY, bool constrainPitch)
{
	pitch_ += offsetY * mouseSensitivity_;
	yaw_ += offsetX * mouseSensitivity_;
	if (constrainPitch)
	{
		if (pitch_ > 89.0f)
			pitch_ = 89.0f;
		if (pitch_ < -89.0f)
			pitch_ = -89.0f;
	}
	updateCameraVectors();
}


void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.y = sin(pitch_);
	front.x = cos(pitch_) * cos(yaw_);
	front.z = cos(pitch_) * sin(yaw_);
	camFront_ = glm::normalize(front);
	camRight_ = glm::normalize(glm::cross(camFront_, worldUp_));
	camUp_ = glm::normalize(glm::cross(camRight_, camFront_));

}
