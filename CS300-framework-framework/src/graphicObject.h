#pragma once

#include "object.h"
#include "models.h"
#include "globals.h"

class graphicObject : public object
{
public:

	enum eTextureType
	{
		DIFFUSE = 0,
		NORMAL,
		SPECULAR,
	};
	graphicObject();
	graphicObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, unsigned int meshType, unsigned int indexCount);
	~graphicObject();
	void update(void(*updateFN)());
	void draw(unsigned int shader);
	void setColor(glm::vec3 col);
	void setTextureMap(unsigned int tex);
	void setSpecularMap(unsigned int spec);
	void setNormalMap(unsigned int norm);
	void setMaterialShininess(float shininess);
	void setObjectType(global::eObjectType type);
	global::eObjectMaterialType getMaterialType();
private:
	unsigned int mesh;
	int meshIndexCount;
	float materialShininess = 120.f;
	global::eObjectMaterialType material;
	global::eObjectType objType;
	int textures[3] = { -1, -1, -1 };
	glm::vec3 color;
	global::eObjectType objectType = global::eObjectType::GAMEOBJECT;
	global::eObjectMaterialType currentMaterial();
};