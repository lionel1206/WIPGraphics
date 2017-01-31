#version 330
in vec4 vertex;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 uv;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * vertex;
    uv = vertex.xyz;
}