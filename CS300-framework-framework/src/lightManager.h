#pragma once

#include <vector>
#include "pointLight.h"
#include "directionalLight.h"

struct ambientLightParam
{
	glm::vec3 ambientLightColor;
	float ambientLightStrength;
};

struct directionalLightParam
{
	glm::vec3 directionLightDir;
	glm::vec3 directionLightDiffuse;
	glm::vec3 directionLightSpecular;
};

struct pointLightParam
{
	glm::vec3 pointLightPosition;
	glm::vec3 pointLightDiffuse;
	glm::vec3 pointLightSpecular;
	float pointLightAttenuationDistance;
	float pointLightAttenuationConstanst;
	float pointLightAttenuationLinear;
	float pointLightAttenuationQuadratic;
};

typedef std::vector<pointLightParam> pointLightParamContainter;
typedef std::vector<directionalLightParam> directionLightParamContainter;

class lightManager
{
public:
	lightManager();
	lightManager(std::vector<pointLight>& ptLights, std::vector<directionalLight>& dirLights);
	~lightManager();
	void updateLightParameters(pointLightParamContainter& ptLightParams, directionLightParamContainter& directionalLightParameters);
	void passDataToShader(unsigned int shader);
	void draw(unsigned int shader);
	std::vector<directionalLight>& getDirectionalLights();
	std::vector<pointLight>& getPointLights();
private:
	std::vector<pointLight> pointLightContainer;
	std::vector<directionalLight> directionalLightContainer;
};
