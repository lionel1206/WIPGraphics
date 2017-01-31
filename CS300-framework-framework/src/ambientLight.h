#pragma once

#include "light.h"
#include "globals.h"

class ambientLight : public light
{
public:
	ambientLight();
	~ambientLight();
	void update(void(*updateFN)());
	void draw(unsigned int shader);
	void updateLightParameter(unsigned int shader);

	void setAmbientColor(glm::vec3 &color);
	void setAmbientStrength(float strength);

	const glm::vec3 getAmbientColor();
	const float getAmbientStrength();

private:
	float ambientStrength;
	global::eObjectType objectType = global::eObjectType::AMBIENT_LIGHT;
};