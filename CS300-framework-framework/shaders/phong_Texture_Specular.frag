#version 330

struct materialStruct
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float materialShininess;
};

struct ambientLightStruct
{
	vec3 diffuse;
	float strength;
};

struct directionLightStruct
{
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct pointLightStruct
{
	vec3 diffuse;
	vec3 specular;
	vec3 position;

	float distance;
	float constant;
	float linear;
	float quadratic;
};

const int max_lights = 10;

in vec3 normalVec, eyeVec, worldVertex;
in vec2 uv;

out vec4 fragColor;

uniform materialStruct material;
uniform ambientLightStruct ambientLight;
uniform directionLightStruct directionLight[max_lights];
uniform pointLightStruct pointLight[max_lights];

vec3 directionLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient);
vec3 pointLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient);

vec3 directionLightCalculation(directionLightStruct dirLight, vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 materialSpecular, vec3 ambient)
{
	vec3 lightVec = -dirLight.direction;
	float dotLN = max(0.0, dot(lightVec, normal));
	vec3 diffuse = dirLight.diffuse * dotLN * materialColor;	
	vec3 halfwayVec = normalize(lightVec + eye);
	float BlinnSpecular = pow(max(dot(normal, halfwayVec), 0.f), materialShininess);
	return (ambient + diffuse + (dirLight.specular * BlinnSpecular * materialSpecular));
}

vec3 pointLightCalculation(pointLightStruct ptLight, vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 materialSpecular, vec3 ambient)
{
	vec3 lightVec = ptLight.position - worldVertex;
	float distance = length(lightVec);
	lightVec = normalize(lightVec);
	float dotLN = max(0.0, dot(lightVec, normal));
	vec3 diffuse = ptLight.diffuse * dotLN * materialColor;	
	vec3 halfwayVec = normalize(lightVec + eye);
	float BlinnSpecular = pow(max(dot(normal, halfwayVec), 0.f), materialShininess);
	float attenuation = 0.f;
	if(distance <  ptLight.distance)
	{
		attenuation = 1.f / (ptLight.constant + ptLight.linear * distance + ptLight.quadratic * (distance * distance));
	}


	return ((ambient * attenuation) + (diffuse * attenuation) + ((ptLight.specular * BlinnSpecular * materialSpecular) * attenuation));
}

void main()
{
    vec4 textureDiffuseColor = texture2D(material.diffuseMap, uv);
	vec4 textureSpecularColor = texture2D(material.specularMap, uv);
    vec3 Ambient = (ambientLight.diffuse * ambientLight.strength) * textureDiffuseColor.xyz;
	vec3 color = vec3(0,0,0);
	for(int i = 0; i < 1; ++i)
	{
		color += directionLightCalculation(directionLight[i], normalVec, eyeVec, textureDiffuseColor.xyz, material.materialShininess, textureSpecularColor.xyz, Ambient);
	}
	
	for(int i = 0; i < max_lights; ++i)
	{
		color += pointLightCalculation(pointLight[i], normalVec, eyeVec, textureDiffuseColor.xyz, material.materialShininess, textureSpecularColor.xyz, Ambient);
	}
    
    
	fragColor = vec4(color,0.0);
}