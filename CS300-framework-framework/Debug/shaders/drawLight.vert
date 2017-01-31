#version 330

uniform mat4 ProjectionMatrix, ViewMatrix, ModelMatrix;
in vec4 vertex;

void main()
{
    gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vertex;
}