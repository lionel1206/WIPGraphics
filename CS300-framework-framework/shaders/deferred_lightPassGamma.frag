#version 330

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

in vec2 uv;
out vec3 finalRenderTexture;

uniform sampler2D gPositionTexture;
uniform sampler2D gNormalTexture;
uniform sampler2D gAlbedoTexture;
uniform sampler2D gSpecularTexture;
uniform vec3 cameraPos;

const int max_lights = 32;

uniform ambientLightStruct ambientLight;
uniform directionLightStruct directionLight[max_lights];
uniform pointLightStruct pointLight[max_lights];

vec3 directionLightCalculation(directionLightStruct dirLight, vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 materialSpecular, vec3 ambient)
{
	vec3 lightVec = -dirLight.direction;
	float dotLN = max(0.0, dot(lightVec, normal));
	vec3 diffuse = dirLight.diffuse * dotLN * materialColor;	
	vec3 halfwayVec = normalize(lightVec + eye);
	float BlinnSpecular = pow(max(dot(normal, halfwayVec), 0.f), materialShininess);
	return (ambient + diffuse + (dirLight.specular * BlinnSpecular * materialSpecular));
}

vec3 pointLightCalculation(pointLightStruct ptLight, vec3 position, vec3 normal, vec3 eye, vec3 materialColor, float materialShininess, vec3 materialSpecular, vec3 ambient)
{
	vec3 lightVec = ptLight.position - position;
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
    vec3 position = texture(gPositionTexture, uv).xyz;
    vec3 normal = texture(gNormalTexture, uv).xyz;
    vec3 textureDiffuseColor = texture(gAlbedoTexture, uv).rgb;
    vec3 textureSpecularColor = texture(gSpecularTexture, uv).rgb;
    float materialShininesss = texture(gSpecularTexture, uv).a * 255.f;
    vec3 Ambient = (ambientLight.diffuse * ambientLight.strength) * textureDiffuseColor;
    vec3 eyeVec = normalize(cameraPos - position);
	vec3 dirLightColor = vec3(0,0,0);
    vec3 ptLightColor = vec3(0,0,0);
	for(int i = 0; i < 1; ++i)
	{
		dirLightColor += directionLightCalculation(directionLight[i], normal, eyeVec, textureDiffuseColor, materialShininesss, textureSpecularColor, Ambient);
	}
	
	for(int i = 0; i < max_lights; ++i)
	{
		ptLightColor += pointLightCalculation(pointLight[i], position, normal, eyeVec, textureDiffuseColor, materialShininesss, textureSpecularColor, Ambient);
	}
    
    
	vec3 color = dirLightColor + ptLightColor;
	vec3 gamma = vec3(1/2.2f);
	finalRenderTexture = pow(color, gamma);
}