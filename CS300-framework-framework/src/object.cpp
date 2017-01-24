#include "object.h"
#include "glm\ext.hpp"
object::object()
{

}

object::~object()
{

}

void object::setPosition(glm::vec3 &pos)
{
	translationMtx = glm::translate(pos);
}

void object::setRotation(glm::vec3 &rotAxis, float deg)
{
	rotationMtx = glm::rotate(deg, rotAxis);
}

void object::setScale(glm::vec3 &scale)
{
	scaleMtx = glm::scale(scale);
}
