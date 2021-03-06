////////////////////////////////////////////////////////////////////////
// A small library of object shapes (ground plane, sphere, and the
// famous Utah teapot), each created as a batch of Vertex Buffer
// Objects collected under a Vertex Array Object umbrella.  This is
// the latest and most efficient way to get geometry into the OpenGL
// graphics pipeline.
//
// Each vertex is specified as four attributes which are made
// available in a vertes shader in the following attribute slots.
//
// position,		vec4,	attribute #0
// normal,			vec3,	attribute #1
// texture coord,	vec3,	attribute #2
// tangent,			vec3,	attribute #3
//
// An instance of any of these shapes is create with a single call:
//    unsigned int obj = CreateSphere(divisions, &quadCount);
// and drawn by:
//	  glBindVertexArray(obj);
//	  glDrawElements(GL_QUADS, quadCount, GL_UNSIGNED_INT, 0);
//	  glBindVertexArray(0);
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <vector>

#include "glm\ext.hpp"

struct meshData
{
	std::string meshName;
	std::vector<glm::vec3> uvs;
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> faces;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tans;
	std::vector<glm::vec3> biTans;
};

typedef std::vector<meshData> modelData;

bool loadModelFromFile(const char *path, meshData &mesh);
unsigned int createVAO(meshData& mesh);
unsigned int createQuad(unsigned int& faceCount);
unsigned int CreateTeapot(const int n,  unsigned int& count);
unsigned int CreateSphere(const int n,  unsigned int& count);
unsigned int CreateGround(const float range, const int n,  unsigned int& count);
