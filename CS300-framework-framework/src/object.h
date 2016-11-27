#pragma once

#include "glm\glm.hpp"

class object
{
public:
	object();
	~object();
	virtual void initialize() = 0;
	virtual void update() = 0;
	void setPosition(glm::vec3 &pos);
	void setRotation(float deg);
	void setScale(glm::vec3 &scale);
private:
	glm::mat4 translationMtx;
	glm::mat4 rotationMtx;
	glm::mat4 scaleMtx;
};

object::object()
{
}

object::~object()
{
}