#include "lightManager.h"

lightManager::lightManager()
{

}

lightManager::lightManager(std::vector<pointLight>& ptLights, std::vector<directionalLight>& dirLights)
{
	for (auto ptLight : ptLights)
	{
		pointLightContainer.push_back(ptLight);
	}

	for (auto dirLight : dirLights)
	{
		directionalLightContainer.push_back(dirLight);
	}

}

lightManager::~lightManager()
{

}

void lightManager::updateLightParameters(pointLightParamContainter& ptLightParams, directionLightParamContainter& directionalLightParameters)
{
	for (unsigned int i = 0; i < pointLightContainer.size(); ++i)
	{
		pointLightContainer[i].setPosition(ptLightParams[i].pointLightPosition);
		pointLightContainer[i].setDiffuseColor(ptLightParams[i].pointLightDiffuse);
		pointLightContainer[i].setSpecularColor(ptLightParams[i].pointLightSpecular);

		pointLightContainer[i].setAttenuationParameters(ptLightParams[i].pointLightAttenuationDistance,
			                                            ptLightParams[i].pointLightAttenuationConstanst,
			                                            ptLightParams[i].pointLightAttenuationLinear,
			                                            ptLightParams[i].pointLightAttenuationQuadratic);

	}

	for (unsigned int i = 0; i < directionalLightContainer.size(); ++i)
	{
		directionalLightContainer[i].setLightDirection(directionalLightParameters[i].directionLightDir);
		directionalLightContainer[i].setDiffuseColor(directionalLightParameters[i].directionLightDiffuse);
		directionalLightContainer[i].setSpecularColor(directionalLightParameters[i].directionLightSpecular);
	}
}

void lightManager::passDataToShader(unsigned int shader)
{
	for (auto ptLight : pointLightContainer)
	{
		ptLight.updateLightParameter(shader);
	}

	for (auto dirLight : directionalLightContainer)
	{
		dirLight.updateLightParameter(shader);
	}
}

void lightManager::draw(unsigned int shader)
{
	for (auto ptLight : pointLightContainer)
	{
		ptLight.draw(shader);
	}
}

std::vector<directionalLight>& lightManager::getDirectionalLights()
{
	return directionalLightContainer;
}

std::vector<pointLight>& lightManager::getPointLights()
{
	return pointLightContainer;
}
