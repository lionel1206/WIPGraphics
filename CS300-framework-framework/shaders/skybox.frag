#version 330

in vec3 uv;
out vec4 fragColor;
uniform samplerCube skybox;

void main()
{
    fragColor  = texture(skybox, uv);
}