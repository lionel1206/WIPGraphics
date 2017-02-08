#version 330

in vec3 uv;
out vec3 finalRenderTexture;
uniform samplerCube skybox;

void main()
{
    finalRenderTexture  = texture(skybox, uv).xyz;
}