#include "light.h"

light::light()
{

}

light::~light()
{

}

void light::setLightColor(glm::vec3 &col)
{
	lightColor = col;
}

void light::setLightIndex(int index)
{
	lightIndex = index;
}
