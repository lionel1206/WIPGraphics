/////////////////////////////////////////////////////////////////////////
// Vertex shader for Phong lighting calculation
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

// These are inputs from the application.
uniform vec3 lightPos;
uniform mat4 ViewInverse, ModelMatrix, NormalMatrix;

// These are inputs directly from the model.
in vec4 vertex;
in vec3 vertexNormal;

// These are outputs from this vertex processor to the individual
// pixel processors.  The values are interpolated diring the
// rasterization step.
out vec3 normalVec, lightVec, eyeVec;

// Calculate the geometric vectors that need interpolating to individual pixels.
void PhongVS()
{
	//transformation of normal vectors
	normalVec = vec3(transpose(inverse(NormalMatrix)) * vec4(vertexNormal, 0.f));
	//getting the vertex world space position from their model position
    vec3 VertexWorldPosition = vec3(ModelMatrix * vertex);
	lightVec = lightPos - VertexWorldPosition;
	//getting the eye look at vector in world space
	eyeVec = vec3(ViewInverse * vec4(0.f, 0.f, 0.f, 1.f)) - VertexWorldPosition;
}
