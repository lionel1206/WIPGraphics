#pragma once

#include "glm\ext.hpp"
#include "glm\glm.hpp"

class camera
{
public:
	camera();
	~camera();
	void initialize(glm::vec3& pos, glm::vec3& lookat);
	void update();
	void zoom(float displacement);
	void horizontalMove(float dx);
	void verticalMove(float dy);
	void setLookAt(glm::vec3 pos);
	void setPanDir(glm::vec2 pan);
	glm::vec3 getPosition();
	glm::mat4 getViewMtx();
private:
	glm::vec3 getLookAtDir();
	glm::vec3 getSideDir();
	glm::vec3 getUpdir();
	float cameraSpeed = 100.f;
	float angleSpeed = 45.f;
	float panSpeed = 10.f;
	glm::vec3 position;
	glm::vec3 lookAtVec;
	glm::vec3 lookAtPt;
	glm::mat4 viewMtx;
};