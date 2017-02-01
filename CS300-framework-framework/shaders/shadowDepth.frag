#version 330

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(vec3(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z), 1.f);
}