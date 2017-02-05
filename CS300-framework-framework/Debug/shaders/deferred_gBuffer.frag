#version 330

layout(location = 0) out vec3 gPositionTexture;
layout(location = 1) out vec3 gNormalTexture;
layout(location = 2) out vec3 gAlbedoTexture;
layout(location = 3) out vec4 gSpecularTexture;

in vec3 worldVertex;
in vec2 uv;
in vec3 normal;

struct materialStruct
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float materialShininess;
};

uniform materialStruct material;

void main()
{
    gPositionTexture = worldVertex;
    gNormalTexture = normal;
    gAlbedoTexture = texture(material.diffuseMap, uv).rgb;
    gSpecularTexture.rgb = vec3(texture(material.specularMap, uv).rgb);
	gSpecularTexture.a = material.materialShininess;
}