#pragma once

#include "object.h"

class light : public object
{
public:
	light();
	~light();
	void setLightColor(glm::vec3 &col);
private:
	glm::vec3 lightColor;
};
