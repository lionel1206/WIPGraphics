/////////////////////////////////////////////////////////////////////////
// Vertex shader for Phong lighting calculation
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

// These are inputs from the application.
uniform vec3 lightPos;
uniform mat4 NormalMatrix;

// These are outputs from this vertex processor to the individual
// pixel processors.  The values are interpolated diring the
// rasterization step.
out vec3 normalVec, lightVec, eyeVec;

// These are inputs from the application.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix, ViewInverse;
uniform mat4 ProjectionMatrix;

// These are inputs directly from the model.
in vec4 vertex;
in vec3 vertexNormal;
in vec2 vertexTexture;
in vec3 vertexTangent;

// Calculate the geometric vectors that need interpolating to individual pixels.
void main()
{
	//transformation of normal vectors
	normalVec = vec3(transpose(inverse(NormalMatrix)) * vec4(vertexNormal, 0.f));
	//getting the vertex world space position from their model position
    vec3 VertexWorldPosition = vec3(ModelMatrix * vertex);
	lightVec = lightPos - VertexWorldPosition;
	//getting the eye look at vector in world space
	eyeVec = vec3(ViewInverse * vec4(0.f, 0.f, 0.f, 1.f)) - VertexWorldPosition;

	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vertex;
	//gl_Position = vertex * ModelMatrix * ViewMatrix * ProjectionMatrix;
}
