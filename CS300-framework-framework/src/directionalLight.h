#pragma once
#include "light.h"
#include "globals.h"

class directionalLight : public light
{
public:
	directionalLight();
	~directionalLight();
	directionalLight(glm::vec3 &dir,
					 glm::vec3 &diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f),
					 glm::vec3 &hilightColor = glm::vec3(0.5f, 0.5f, 0.5f));
	void update(void(*updateFN)());
	void draw(unsigned int shader);
	void updateLightParameter(unsigned int shader);

	const glm::vec3& getLightDirection();
	void setLightDirection(glm::vec3& dir);

	void setDiffuseColor(glm::vec3 &diffuse);
	void setSpecularColor(glm::vec3 &specular);

	const glm::vec3 getDiffuseColor();
	const glm::vec3 getSpecularColor();

private:
	global::eObjectType objectType = global::eObjectType::DIRECTIONAL_LIGHT;
	glm::vec3 lightDir;
	glm::vec3 specularColor;
};