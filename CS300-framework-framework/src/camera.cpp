#include "camera.h"

camera::camera()
{

}

camera::~camera()
{

}

void camera::initialize(float _angle, float _fi, float _radius, glm::vec3 lookat)
{
	angle = _angle;
	fi = _fi;
	radius = _radius;
	lookAtPt = lookat;
}

void camera::setAngle(float deg)
{
	angle = deg;
}

void camera::setFi(float deg)
{
	fi = deg;
}

void camera::setRadius(float r)
{
	radius = r;
}

void camera::setLookAt(glm::vec3 pos)
{
	lookAtPt = pos;
}

glm::mat4 camera::getViewMtx()
{
	return viewMtx;
}

void camera::update()
{
	glm::vec3 cameraPosition;
	cameraPosition.x = radius * glm::cos(angle) * glm::sin(fi);
	cameraPosition.y = radius * glm::sin(angle) * glm::sin(fi);
	cameraPosition.z = radius * glm::cos(fi);

	viewMtx = glm::lookAt(cameraPosition, lookAtPt, glm::vec3(0, 1, 0));
}
