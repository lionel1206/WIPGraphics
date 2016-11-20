/////////////////////////////////////////////////////////////////////////
// Vertex shader for the final pass
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

// Prototype for the Phong calculation procedure
void PhongVS();

// These are inputs from the application.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix, ViewInverse;
uniform mat4 ProjectionMatrix;

// These are inputs directly from the model.
in vec4 vertex;

void main()
{      
	PhongVS(); // Setup phong lighting
	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vertex;
}
