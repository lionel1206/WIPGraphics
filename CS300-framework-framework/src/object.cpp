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
	translate = pos;
}

void object::setRotation(glm::vec3 &rot)
{
	rotate = rot;
}

void object::setScale(glm::vec3 &s)
{
	scale = s;
}

void object::setIsShadowCaster(bool flag)
{
	isShadowCaster = flag;
}

void object::setIsShadowReceiver(bool flag)
{
	isShadowReceiver = flag;
}

void object::setIsVisible(bool flag)
{
	isVisible = flag;
}

void object::setName(char* name)
{
	objectName = name;
}

glm::vec3 object::getTranslation()
{
	return translate;
}

glm::vec3 object::getRotation()
{
	return rotate;
}

glm::vec3 object::getScale()
{
	return scale;
}

bool object::getIsShadowCaster()
{
	return isShadowCaster;
}

bool object::getIsShadowReceiver()
{
	return isShadowReceiver;
}

bool object::getIsVisible()
{
	return isVisible;
}

const char* object::getName()
{
	return objectName.c_str();
}
