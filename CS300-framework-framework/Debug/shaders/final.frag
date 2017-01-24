/////////////////////////////////////////////////////////////////////////
// Pixel shader for the final pass
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

// These are inputs from the application.
uniform int WIDTH;
uniform int HEIGHT;
uniform int mode;

// These are inputs from the resterization step.  The values are
// interpolated from the vertex processor outputs of the same name.
in vec3 normalVec, lightVec, eyeVec;

// Prototype for the Phong calculation procedure
vec4 PhongFS(vec3 N, vec3 L, vec3 E);

void main()
{
	// Calculate and output the phong lighting.
	gl_FragColor = PhongFS(normalVec, lightVec, eyeVec);
}
