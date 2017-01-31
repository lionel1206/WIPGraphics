#pragma once
#include "light.h"
#include "globals.h"

class pointLight : public light
{
public:
	pointLight();
	~pointLight();
	pointLight(glm::vec3 &position,
		       unsigned int meshType, unsigned int indexCount,
		       glm::vec3 &diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f),
		       glm::vec3 &hilightColor = glm::vec3(0.5f, 0.5f, 0.5f));

	void update(void(*updateFN)());
	void draw(unsigned int shader);
	void updateLightParameter(unsigned int shader);
	void setDiffuseColor(glm::vec3 &diffuse);
	void setSpecularColor(glm::vec3 &specular);

	const glm::vec3 getDiffuseColor();
	const glm::vec3 getSpecularColor();

	void setAttenuationParameters(float dist, float constant, float linear, float quad);
	void getAttenuationParameters(float &dist, float &constant, float &linear, float &quad);
private:
	global::eObjectType objectType = global::eObjectType::POINT_LIGHT;
	unsigned int mesh;
	int meshIndexCount;
	glm::vec3 specularColor;

	float attenuationDistance = 100.f;
	float attenutationConstant = 1.f;
	float attenutationLinear = 0.045f;
	float attenuationQuadratic = 0.0075f;
};