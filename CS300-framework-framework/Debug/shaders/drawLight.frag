#version 330

uniform vec3 diffuse;
out vec4 fragColor;
void main()
{
    fragColor  = vec4(diffuse, 0.f);
}