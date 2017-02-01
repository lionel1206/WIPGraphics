#version 330

in vec2 uv;
uniform sampler2D shadowMapDepth;
out vec4 color;

void main()
{
	color = texture(shadowMapDepth, uv);
}