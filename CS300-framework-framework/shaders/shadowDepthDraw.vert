#version 330

in vec4 vertex;
in vec3 vertexTexture;

out vec2 uv;

void main()
{
	gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f); 
    uv = vertexTexture.xy;
}