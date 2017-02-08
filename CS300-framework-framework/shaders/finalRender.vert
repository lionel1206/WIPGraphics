#version 330

in vec3 vertex;
in vec2 vertexTexture;
uniform mat4 transform;
out vec2 uv;

void main()
{
	gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f); 
    uv = vertexTexture;
}