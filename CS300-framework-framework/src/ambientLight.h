#pragma once

#include "light.h"

class ambientLight : public light
{
public:
	ambientLight();
	~ambientLight();
	void initialize();
	void update();
private:
	glm::vec3 ambientColor;
};