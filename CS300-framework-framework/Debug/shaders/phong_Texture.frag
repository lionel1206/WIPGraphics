#version 330

struct materialStruct
{
	sampler2D diffuseMap;
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

in vec3 normalVec, eyeVec, worldVertex;
in vec2 uv;

out vec4 finalColor;

uniform materialStruct material;
uniform ambientLightStruct ambientLight;
uniform directionLightStruct directionLight;
uniform pointLightStruct pointLight;

vec3 directionLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient);
vec3 pointLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient);

vec3 directionLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient)
{
	vec3 lightVec = -directionLight.direction;
	float dotLN = max(0.0, dot(lightVec, normal));
	vec3 diffuse = directionLight.diffuse * dotLN * materialColor;	
	vec3 halfwayVec = normalize(lightVec + eye);
	float BlinnSpecular = pow(max(dot(normal, halfwayVec), 0.f), materialShininess);
	return (ambient + diffuse + (directionLight.specular * BlinnSpecular));
}

vec3 pointLightCalculation(vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 ambient)
{
	vec3 lightVec = pointLight.position - worldVertex;
	float distance = length(lightVec);
	lightVec = normalize(lightVec);
	float dotLN = max(0.0, dot(lightVec, normal));
	vec3 diffuse = pointLight.diffuse * dotLN * materialColor;	
	vec3 halfwayVec = normalize(lightVec + eye);
	float BlinnSpecular = pow(max(dot(normal, halfwayVec), 0.f), materialShininess);
	float attenuation = 0.f;
	if(distance <  pointLight.distance)
	{
		attenuation = 1.f / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	}


	return ((ambient * attenuation) + (diffuse * attenuation) + ((pointLight.specular * BlinnSpecular) * attenuation));
}

void main()
{
    vec4 textureDiffuseColor = texture2D(material.diffuseMap, uv);
    vec3 Ambient = (ambientLight.diffuse * ambientLight.strength) * textureDiffuseColor.xyz;
	vec3 color = directionLightCalculation(normalVec, eyeVec, textureDiffuseColor.xyz, material.materialShininess, Ambient);
    color += pointLightCalculation(normalVec, eyeVec, textureDiffuseColor.xyz, material.materialShininess, Ambient);
    
	finalColor = vec4(color,0.0);
}