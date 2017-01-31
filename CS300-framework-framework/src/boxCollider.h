#pragma once

#include "collider.h"

class boxCollider
{
public:
	boxCollider();
	~boxCollider();

	void createCollisionVolume(glm::vec3 *vertices, int count);
	bool intersect(ray &inRay, float& t);
	void update(glm::mat4 modelToWorldMtx);

private:
	glm::vec3 min;
	glm::vec3 max;
};

