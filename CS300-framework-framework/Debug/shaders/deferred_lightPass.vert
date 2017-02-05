#version 330

in vec3 vertex;
in vec2 vertexTexture;
out vec2 uv;

void main()
{
    gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f);
    uv = vertexTexture;
}