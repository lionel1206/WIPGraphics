#include "camera.h"

camera::camera()
{

}

camera::~camera()
{

}

void camera::initialize(glm::vec3& pos, glm::vec3& lookat)
{
	position = pos;
	lookAtPt = lookat;
	lookAtVec = glm::normalize(lookAtPt - position);
}

void camera::setLookAt(glm::vec3 pos)
{
	lookAtPt = pos;
}

void camera::zoom(float displacement)
{
	glm::vec3 lookDir = position - lookAtPt;
	float dist = glm::length(lookDir) - displacement * cameraSpeed;
	position = lookAtPt + (glm::normalize(lookDir) * dist);
}

void camera::horizontalMove(float dx)
{
	position = glm::angleAxis(-dx * angleSpeed, glm::vec3(0, 1, 0)) * position;
}

void camera::verticalMove(float dy)
{
	glm::vec3 sideVec = glm::normalize(glm::cross(getLookAtDir(), glm::vec3(0, -1, 0)));
	position = glm::angleAxis(dy * angleSpeed, sideVec) * position;
}

glm::mat4 camera::getViewMtx()
{
	return viewMtx;
}

void camera::update()
{
	viewMtx = glm::lookAt(position, lookAtPt, glm::vec3(0, 1, 0));
}

glm::vec3 camera::getPosition()
{
	return position;
}

void camera::setPanDir(glm::vec2 pan)
{
	float direction = 1.f;
	if (glm::dot(lookAtPt - position, glm::vec3(0, 0, 1)) < 0.f)
	{
		direction = -1.f;
	}

	glm::vec3 right = getSideDir();
	glm::vec3 up = getUpdir();

	position += right * pan.x * panSpeed;
	lookAtPt += right * pan.x * panSpeed;

	position += up * pan.y * panSpeed;
	lookAtPt += up * pan.y * panSpeed;
}

glm::vec3 camera::getLookAtDir()
{
	return lookAtPt - position;
}

glm::vec3 camera::getSideDir()
{
	return glm::normalize(glm::cross(getLookAtDir(), glm::vec3(0, -1, 0)));
}

glm::vec3 camera::getUpdir()
{
	return glm::normalize(glm::cross(glm::normalize(getLookAtDir()), getSideDir()));
}

