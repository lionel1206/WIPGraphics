#version 330

in vec4 position;
layout(location = 0) out vec3 color;

void main()
{
	color = vec3(position.w);
}