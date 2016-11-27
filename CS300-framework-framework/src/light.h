#pragma once

#include "object.h"

class light : public object
{
public:
	light();
	~light();

private:
	glm::vec3 lightColor;
};
