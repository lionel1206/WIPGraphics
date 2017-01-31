#include "pointLight.h"
#include "GL\glew.h"
#include "glm\ext.hpp"
#include <sstream>

pointLight::pointLight()
{

}

pointLight::~pointLight()
{

}

pointLight::pointLight(glm::vec3 &pos, unsigned int meshType, unsigned int indexCount,
	glm::vec3 &diffuseColor, glm::vec3 &hilightColor)
{
	setPosition(pos);
	lightColor = diffuseColor;
	specularColor = hilightColor;
	mesh = meshType;
	meshIndexCount = indexCount;
}

void pointLight::update(void(*updateFN)())
{
	if (updateFN != nullptr)
		updateFN();
}

void pointLight::updateLightParameter(unsigned int shader)
{
	std::stringstream ss;
	ss << "pointLight[" << lightIndex << "].";

	int loc = glGetUniformLocation(shader, (ss.str() + "diffuse").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));

	loc = glGetUniformLocation(shader, (ss.str() + "position").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(getTranslation()));

	loc = glGetUniformLocation(shader, (ss.str() + "specular").c_str());
	glUniform3fv(loc, 1, glm::value_ptr(specularColor));

	loc = glGetUniformLocation(shader, (ss.str() + "distance").c_str());
	glUniform1f(loc, attenuationDistance);

	loc = glGetUniformLocation(shader, (ss.str() + "constant").c_str());
	glUniform1f(loc, attenutationConstant);

	loc = glGetUniformLocation(shader, (ss.str() + "linear").c_str());
	glUniform1f(loc, attenutationLinear);

	loc = glGetUniformLocation(shader, (ss.str() + "quadratic").c_str());
	glUniform1f(loc, attenuationQuadratic);
}

void pointLight::draw(unsigned int shader)
{
	
	glm::mat4 translate = glm::translate(getTranslation());
	glm::mat4 scale = glm::scale(glm::vec3(5, 5, 5));
	glm::mat4 modelMtx  = translate * scale;
	int loc = glGetUniformLocation(shader, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(modelMtx));
	loc = glGetUniformLocation(shader, "diffuse");
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));
	glBindVertexArray(mesh);
	glDrawElements(GL_TRIANGLES, meshIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void pointLight::setDiffuseColor(glm::vec3 &diffuse)
{
	lightColor = diffuse;
}

void pointLight::setSpecularColor(glm::vec3 &specular)
{
	specularColor = specular;
}

const glm::vec3 pointLight::getDiffuseColor()
{
	return lightColor;
}

const glm::vec3 pointLight::getSpecularColor()
{
	return specularColor;
}

void pointLight::setAttenuationParameters(float dist, float constant, float linear, float quad)
{
	attenuationDistance = dist;
	attenutationConstant = constant;
	attenutationLinear = linear;
	attenuationQuadratic = quad;
}

void pointLight::getAttenuationParameters(float &dist, float &constant, float &linear, float &quad)
{
	dist = attenuationDistance;
	constant = attenutationConstant;
	linear = attenutationLinear;
	quad = attenuationQuadratic;
}
