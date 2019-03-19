#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BoundingBox.h"

enum MoveCamera {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


const float MOUSE_SENSITIVITY = 0.1f;
const float MOVEMENT_SPEED = 25.0f;


class Camera
{
public:

	Camera(float windowWidth, float windowHeight);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	void setProjectionMatrix(glm::mat4 projectionMatrix);

	void move(glm::vec3 tomove);

	void fpKeyboardMove(MoveCamera direction, float dt);
	void fpMouseMove(float dx, float dy, bool constrainPitch = true);

	glm::vec3 getPosition();

	BoundingBox box;

private:

	glm::mat4 projection = glm::mat4(1);

	float windowWidth, windowHeight;

	glm::vec3 cameraPos = glm::vec3(0.0f, 32.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = 90.0f;
	float pitch = 0.0f;

	void updateCameraAttributes();
};

#endif