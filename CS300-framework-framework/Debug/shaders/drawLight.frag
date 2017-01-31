#version 330

uniform vec3 diffuse;
out vec4 finalCOlor;

void main()
{
    finalCOlor = vec4(diffuse, 0.f);
}