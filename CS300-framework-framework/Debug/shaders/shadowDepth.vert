#version 330

in vec4 vertex;
uniform mat4 LightSpaceMtx, ModelMatrix;
out vec4 position;

void main()
{
	gl_Position = LightSpaceMtx * ModelMatrix * vertex;
	position = gl_Position;
}