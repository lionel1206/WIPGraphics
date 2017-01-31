#pragma once

#include "ray.h"

class collider
{
public:
	virtual void createCollisionVolume(glm::vec3 *vertices, int count) = 0;
	virtual bool intersect(ray &inRay, float& t) = 0;
	virtual void update(glm::mat4 modelToWorldMtx) = 0;
};