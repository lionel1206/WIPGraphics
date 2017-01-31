#include "ambientLight.h"
#include "GL\glew.h"
#include "glm\ext.hpp"

ambientLight::ambientLight()
{

}

ambientLight::~ambientLight()
{

}

void ambientLight::setAmbientColor(glm::vec3 &color)
{
	lightColor = color;
}

void ambientLight::setAmbientStrength(float strength)
{
	ambientStrength = glm::clamp(strength, 0.f, 1.f);
}

const glm::vec3 ambientLight::getAmbientColor()
{
	return lightColor;
}

const float ambientLight::getAmbientStrength()
{
	return ambientStrength;
}

void ambientLight::update(void(*updateFN)())
{
	if (updateFN != nullptr)
		updateFN();
}

void ambientLight::updateLightParameter(unsigned int shader)
{
	int loc = glGetUniformLocation(shader, "ambientLight.diffuse");
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));

	loc = glGetUniformLocation(shader, "ambientLight.strength");
	glUniform1f(loc, ambientStrength);
}

void ambientLight::draw(unsigned int shader)
{
	
}
