#include "directionalLight.h"
#include "GL\glew.h"
#include "glm\ext.hpp"
#include <sstream>

directionalLight::directionalLight()
{

}

directionalLight::~directionalLight()
{

}

directionalLight::directionalLight(glm::vec3 &dir,
								   glm::vec3 &diffuseColor,
								   glm::vec3 &hilightColor)
{
	lightDir = dir;
	lightColor = diffuseColor;
	specularColor = hilightColor;
}

void directionalLight::update(void(*updateFN)())
{
	if (updateFN != nullptr)
		updateFN();
}

void directionalLight::updateLightParameter(unsigned int shader)
{
	std::stringstream ss;
	ss << "directionLight[" << lightIndex << "].";

	int loc = glGetUniformLocation(shader, (ss.str() + "diffuse").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));

	loc = glGetUniformLocation(shader, (ss.str() + "direction").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(lightDir));

	loc = glGetUniformLocation(shader, (ss.str() + "specular").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(specularColor));
}

void directionalLight::draw(unsigned int shader)
{
	
}

const glm::vec3& directionalLight::getLightDirection()
{
	return lightDir;
}

void directionalLight::setLightDirection(glm::vec3& dir)
{
	lightDir = dir;
}

void directionalLight::setDiffuseColor(glm::vec3 &diffuse)
{
	lightColor = diffuse;
}

void directionalLight::setSpecularColor(glm::vec3 &specular)
{
	specularColor = specular;
}

const glm::vec3 directionalLight::getDiffuseColor()
{
	return lightColor;
}

const glm::vec3 directionalLight::getSpecularColor()
{
	return specularColor;
}
