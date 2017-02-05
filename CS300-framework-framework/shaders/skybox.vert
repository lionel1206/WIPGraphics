#version 330
in vec4 vertex;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 uv;

void main()
{
	vec4 position = ProjectionMatrix * ViewMatrix * vertex; 
    gl_Position = position.xyww;
    uv = vertex.xyz;
}