/////////////////////////////////////////////////////////////////////////
// Pixel shader for Phong lighting calculation
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

uniform vec3 lightValue, lightAmbient; // I and Ia
uniform vec3 phongEmitted;             // Ie
uniform vec3 phongDiffuse;			   // Kd
uniform vec3 phongSpecular;			   // Ks
uniform float phongShininess;		   // n (exponent)
// Calcualte and return a lighting value
vec4 PhongFS(vec3 N, vec3 L, vec3 E)
{
	// A reminder to normalize all vectors before calculation
	E = normalize(E);
	L = normalize(L);
	N = normalize(N);
    
	// The ever-present N-dot-L dot product (clamped to positive values)
	float dotLN = max(0.0, dot(L, N));
	
	//getting the normalized reflected vector
	//The reason why we use an unclamped dot product of L and N is because
	//if we were to look from behind an object, we would still see light 
	//from the the clamped dot product while the unclamped result will
	//block out the light
    vec3 R = normalize(2 * dot(L, N) * N - L);
	
	//calculating the components of the specular in the phong equation
    float dotRV = max(0.0, dot(R, E));
    float dotRVpwrn = pow(dotRV, phongShininess);
   
   //calculating the Ambient lighting of the phong equation
    vec3 Ambient = lightAmbient * phongDiffuse;
	
	//calculating the Specular lighting of the phong equation
    vec3 Specular = lightValue * phongSpecular * dotRVpwrn;
	
	//calculating the Ambient lighting of the phong equation
    vec3 Diffuse = dotLN * lightValue * phongDiffuse;	
	
	//The phong equation
    vec3 color = phongEmitted + Ambient + Diffuse + Specular;
    
    //Blinn-Phong, comment the 2 lines below
    //vec3 BlinnSpecular = phongSpecular * lightValue * pow(max(dot(N, normalize(L + E)), 0.f), phongShininess);
    //color = phongEmitted + Ambient + Diffuse + BlinnSpecular;
    
	return vec4(color,0.0);
}
