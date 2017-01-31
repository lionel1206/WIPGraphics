#pragma once

#include "glm\glm.hpp"
#include <string>

class object
{
public:
	object();
	~object();
	virtual void update(void(*updateFN)()) = 0;
	virtual void draw(unsigned int shader) = 0;
	void setPosition(glm::vec3 &pos);
	void setRotation(glm::vec3 &rot);
	void setScale(glm::vec3 &s);
	void setIsShadowCaster(bool flag);
	void setIsShadowReceiver(bool flag);
	void setIsVisible(bool flag);
	void setName(char* name);

	glm::vec3 getTranslation();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	bool getIsShadowCaster();
	bool getIsShadowReceiver();
	bool getIsVisible();
	const char* getName();
protected:
	std::string objectName;
	glm::vec3 translate;
	glm::vec3 rotate;
	glm::vec3 scale;
	bool isShadowCaster = false;
	bool isShadowReceiver = false;
	bool isVisible = true;
};