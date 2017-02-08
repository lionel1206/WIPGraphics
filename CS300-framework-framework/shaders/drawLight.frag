#version 330

uniform vec3 diffuse;
out vec3 finalRenderTexture;
void main()
{
    finalRenderTexture  = diffuse;
}