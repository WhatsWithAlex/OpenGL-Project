#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 global_up(0.0, 1.0, 0.0);
glm::vec3 global_right(1.0, 0.0, 0.0);

class Camera 
{
	glm::vec3 position, direction, up, right, target;
	glm::mat4 look_at;
	GLfloat speed, sensitivity;
	GLfloat pitch, yaw;
	bool target_locked;
	void changeLookAt();
public:
	Camera(glm::vec3 position, glm::vec3 direction, bool target_locked, GLfloat speed, GLfloat sensitivity);
	void setPos(glm::vec3 new_position);
	void setDir(glm::vec3 new_direction);
	void setTarget(glm::vec3 new_target);
	void lockOnTarget(glm::vec3 new_target);
	void lock();
	void unlock();
	void changeLock();
	void move(glm::vec3 shift);
	void rotate(GLfloat angle, glm::vec3 axis);
	void processKeyboard(GLFWwindow *window);
	friend void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void processMouse(GLfloat offset_x, GLfloat offset_y);
	void processScroll(GLFWwindow* window);
	bool isLocked();
	glm::vec3 getPos();
	glm::vec3 getDir();
	glm::mat4 getLookAt();
};

void Camera::changeLookAt() {
	if (target_locked)
		if (position != target) 
			direction = -glm::normalize(position - target);
		else 
			direction = glm::vec3(0.0, 0.0, 1.0);
		
	if (abs(glm::dot(direction, global_up)) <= 0.99999f) 
	{
		right = glm::normalize(glm::cross(global_up, -direction));
		up = glm::cross(-direction, right);
	} 
	else 
	{
		up = glm::normalize(glm::cross(-direction, global_right));
		right = -glm::cross(-direction, up);
	}
	glm::mat4 translation = glm::mat4(1.0f); 
	translation[3][0] = -position.x; 
	translation[3][1] = -position.y;
	translation[3][2] = -position.z;
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0][0] = right.x;
	rotation[1][0] = right.y;
	rotation[2][0] = right.z;
	rotation[0][1] = up.x;
	rotation[1][1] = up.y;
	rotation[2][1] = up.z;
	rotation[0][2] = -direction.x;
	rotation[1][2] = -direction.y;
	rotation[2][2] = -direction.z;
	look_at = rotation * translation;
}
Camera::Camera(glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 target = glm::vec3(0.0, 0.0, 0.0), bool locked = false, GLfloat speed = 0.05f, GLfloat sensitivity = 0.05f)
	: position(position), target(target), target_locked(locked), speed(speed), sensitivity(sensitivity)
{
	if (position != target)
		direction = -glm::normalize(position - target);
	else
		direction = glm::vec3(0.0, 0.0, 1.0);
	changeLookAt();
}
void Camera::setPos(glm::vec3 new_position) { position = new_position; }
void Camera::setDir(glm::vec3 new_direction) { direction = -glm::normalize(new_direction); }
void Camera::setTarget(glm::vec3 new_target) { target = new_target; direction = -glm::normalize(position - target); }
void Camera::lockOnTarget(glm::vec3 new_target) { target = new_target; target_locked = true; }
void Camera::lock() { target_locked = true; }
void Camera::unlock() { target_locked = false; direction = glm::normalize(position - target); }
void Camera::changeLock() 
{
	target_locked = !target_locked;
	if (!target_locked)
		direction = glm::normalize(position - target);
}
void Camera::processMouse(GLfloat offset_x, GLfloat offset_y) {
	yaw += sensitivity * offset_x;
	pitch -= sensitivity * offset_y;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = 89.0f;
	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(dir);
	changeLookAt();
}
void Camera::processKeyboard(GLFWwindow *window) 
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += speed * direction;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= speed * direction;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += up * speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * speed;

	changeLookAt();
}
void Camera::move(glm::vec3 shift) { position = position + shift; changeLookAt(); }
void Camera::rotate(GLfloat angle, glm::vec3 axis) 
{
	glm::mat4 rotate_matrix(1.0f);
	rotate_matrix = glm::rotate(rotate_matrix, angle, axis);
	direction = rotate_matrix * glm::vec4(direction, 1.0);
	changeLookAt();
}
bool Camera::isLocked() { return target_locked; }
glm::vec3 Camera::getPos() { return position; }
glm::vec3 Camera::getDir() { return direction; }
glm::mat4 Camera::getLookAt() { return look_at; }
