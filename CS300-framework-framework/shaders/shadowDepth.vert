#version 330

in vec4 vertex;
uniform mat4 LightSpaceMtx, ModelMatrix;

void main()
{
	gl_Position = LightSpaceMtx * ModelMatrix * vertex;
}