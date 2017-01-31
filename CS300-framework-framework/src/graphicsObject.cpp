#include "graphicObject.h"
#include "GL\glew.h"
#include "glm\ext.hpp"

graphicObject::graphicObject()
{

}

graphicObject::graphicObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, unsigned int meshType, unsigned int indexCount)
{
	translate = pos;
	rotate = rot;
	scale = sca;
	mesh = meshType;
	material = currentMaterial();
	meshIndexCount = indexCount;
}

graphicObject::~graphicObject()
{

}

void graphicObject::update(void(*updateFN)())
{
	if (updateFN != nullptr)
		updateFN();
}

void graphicObject::draw(unsigned int shader)
{
	glm::mat4 translateMtx = glm::translate(translate);
	glm::mat4 rotateMtx = glm::gtc::quaternion::mat4_cast(glm::quat(rotate));
	glm::mat4 scaleMtx = glm::scale(scale);

	glm::mat4 modelToWorldMtx = translateMtx * rotateMtx * scaleMtx;
	int loc = glGetUniformLocation(shader, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(modelToWorldMtx));

	loc = glGetUniformLocation(shader, "NormalMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::inverse(modelToWorldMtx)));

	if (material == global::eObjectMaterialType::COLOR)
	{
		int dloc = glGetUniformLocation(shader, "phongDiffuse");
		glUniform3fv(dloc, 1, glm::value_ptr(color));
	}
	else if (material == global::eObjectMaterialType::TEXTURE)
	{
		glActiveTexture(GL_TEXTURE0);    
		glBindTexture(GL_TEXTURE_2D, textures[eTextureType::DIFFUSE]);
		loc = glGetUniformLocation(shader, "material.diffuseMap");
		glUniform1i(loc, 0);
	}
	
	loc = glGetUniformLocation(shader, "material.materialShininess");
	glUniform1f(loc, materialShininess);

	glBindVertexArray(mesh);
	glDrawElements(GL_TRIANGLES, meshIndexCount, GL_UNSIGNED_INT, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}

void graphicObject::setColor(glm::vec3 col)
{
	color = col;
}

void graphicObject::setTextureMap(unsigned int tex)
{
	textures[eTextureType::DIFFUSE] = tex;
	material = currentMaterial();
}

void graphicObject::setSpecularMap(unsigned int spec)
{
	textures[eTextureType::SPECULAR] = spec;
	material = currentMaterial();
}

void graphicObject::setNormalMap(unsigned int norm)
{
	textures[eTextureType::NORMAL] = norm;
	material = currentMaterial();
}

global::eObjectMaterialType graphicObject::getMaterialType()
{
	return material;
}

void graphicObject::setObjectType(global::eObjectType type)
{
	objectType = type;
}

global::eObjectMaterialType graphicObject::currentMaterial()
{
	if (objectType == global::eObjectType::SKYBOX)
	{
		return global::eObjectMaterialType::TEXTURE_SKYBOX;
	}

	if (textures[eTextureType::DIFFUSE] != -1 &&
		textures[eTextureType::NORMAL] == -1 &&
		textures[eTextureType::SPECULAR] == -1)
	{
		return global::eObjectMaterialType::TEXTURE;
	}

	if (textures[eTextureType::DIFFUSE] != -1 &&
		textures[eTextureType::NORMAL] != -1 &&
		textures[eTextureType::SPECULAR] == -1)
	{
		return global::eObjectMaterialType::TEXTURE_NORMAL;
	}

	if (textures[eTextureType::DIFFUSE] != -1 &&
		textures[eTextureType::NORMAL] != -1 &&
		textures[eTextureType::SPECULAR] != -1)
	{
		return global::eObjectMaterialType::TEXTURE_NORMAL_SPECULAR;
	}

	if (textures[eTextureType::DIFFUSE] != -1 &&
		textures[eTextureType::NORMAL] == -1 &&
		textures[eTextureType::SPECULAR] != -1)
	{
		return global::eObjectMaterialType::TEXTURE_SPECULAR;
	}

	return global::eObjectMaterialType::COLOR;
}

void graphicObject::setMaterialShininess(float shininess)
{
	materialShininess = shininess;
}

