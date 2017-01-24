#pragma once

#include "glm\glm.hpp"

class object
{
public:
	object();
	~object();
	virtual void initialize() = 0;
	virtual void update() = 0;
	void setPosition(glm::vec3 &pos);
	void setRotation(glm::vec3 &rotAxis, float deg);
	void setScale(glm::vec3 &scale);
	void setIsShadowCaster(bool flag);
	void setIsShadowReceiver(bool flag);
	void setIsVisible(bool flag);

	glm::mat4 getTranslationMtx();
	glm::mat4 getRotationMtx();
	glm::mat4 getScaleMtx();
	bool getIsShadowCaster();
	bool getIsShadowReceiver();
	bool getIsVisible();
private:
	glm::mat4 translationMtx;
	glm::mat4 rotationMtx;
	glm::mat4 scaleMtx;
	bool isShadowCaster = false;
	bool isShadowReceiver = false;
	bool isVisible = true;
};