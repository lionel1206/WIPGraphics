#pragma once

#include "glm\ext.hpp"
#include "glm\glm.hpp"

class camera
{
public:
	camera();
	~camera();
	void initialize(float _angle, float _fi, float _radius, glm::vec3 lookat);
	void update();
	void setAngle(float deg);
	void setFi(float deg);
	void setRadius(float r);
	void setLookAt(glm::vec3 pos);
	glm::mat4 getViewMtx();
private:
	// left right movement
	float angle;
	// up down movement
	float fi;
	// distance from lookat
	float radius;
	// look at position
	glm::vec3 lookAtPt;
	glm::mat4 viewMtx;
};