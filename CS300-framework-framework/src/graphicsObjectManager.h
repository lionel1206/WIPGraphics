#pragma once

#include "graphicObject.h"
#include <vector>

struct graphicsObjectParam
{
	glm::vec3 objectTranslate;
	glm::vec3 objectRotate;
	glm::vec3 objectScale;
};

struct sceneInfo
{
	glm::mat4 scenePerspectiveMatrix;
	glm::mat4 sceneViewMatrix;
	glm::mat4 sceneViewInverseMatrix;
	glm::vec3 sceneCameraPosition;
};

class graphicsObjectManager
{
public:
	graphicsObjectManager();
	~graphicsObjectManager();
	graphicsObjectManager(std::vector<graphicObject>& objectContainer);
	void updateSceneParameters(glm::mat4& P, glm::mat4& V, glm::mat4& IV, glm::vec3 &camPos);
	void updateGraphicsObjectParam(std::vector<graphicsObjectParam> graphicsObjectParams);
	void draw(unsigned int shader);
private:
	std::vector<graphicObject> graphicsObjects;
	sceneInfo sceneInformation;
};