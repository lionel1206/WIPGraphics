#include "graphicsObjectManager.h"

graphicsObjectManager::graphicsObjectManager()
{

}

graphicsObjectManager::~graphicsObjectManager()
{

}

graphicsObjectManager::graphicsObjectManager(std::vector<graphicObject>& objectContainer)
{
	for (auto obj : objectContainer)
	{
		graphicsObjects.push_back(obj);
	}
}

void graphicsObjectManager::updateSceneParameters(glm::mat4& P, glm::mat4& V, glm::mat4& IV, glm::vec3 &camPos)
{
	sceneInformation.scenePerspectiveMatrix = P;
	sceneInformation.sceneViewMatrix = V;
	sceneInformation.sceneViewInverseMatrix = glm::inverse(V);
	sceneInformation.sceneCameraPosition = camPos;
}

void graphicsObjectManager::updateGraphicsObjectParam(std::vector<graphicsObjectParam> graphicsObjectParams)
{
	for (unsigned int i = 0; i < graphicsObjects.size(); ++i)
	{
		graphicsObjects[i].setPosition(graphicsObjectParams[i].objectTranslate);
		graphicsObjects[i].setRotation(graphicsObjectParams[i].objectRotate);
		graphicsObjects[i].setScale(graphicsObjectParams[i].objectScale);
	}
}

void graphicsObjectManager::draw(unsigned int shader)
{
	for (auto obj : graphicsObjects)
	{
		obj.draw(shader);
	}
}


