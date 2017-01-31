#pragma once

#include "object.h"

class light : public object
{
public:
	light();
	~light();
	virtual void updateLightParameter(unsigned int shader) = 0;
	void setLightColor(glm::vec3 &col);
	void setLightIndex(int index);
protected:
	glm::vec3 lightColor;
	int lightIndex = 0;
};
