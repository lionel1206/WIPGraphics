/////////////////////////////////////////////////////////////////////////
// Pixel shader for Phong lighting calculation
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#version 330

// These are inputs from the resterization step.  The values are
// interpolated from the vertex processor outputs of the same name.
in vec3 normalVec, lightVec, eyeVec;

uniform vec3 lightValue, lightAmbient; // I and Ia
uniform vec3 phongEmitted;             // Ie
uniform vec3 phongDiffuse;			   // Kd
uniform vec3 phongSpecular;			   // Ks
uniform float phongShininess;		   // n (exponent)
uniform float lightAmbientStrength;

out vec4 finalColor;

// Calcualte and return a lighting value
void main()
{
	// A reminder to normalize all vectors before calculation
	vec3 E = normalize(eyeVec);
	vec3 L = normalize(lightVec);
	vec3 N = normalize(normalVec);
    
	// The ever-present N-dot-L dot product (clamped to positive values)
	float dotLN = max(0.0, dot(L, N));
	
   //calculating the Ambient lighting of the phong equation
    vec3 Ambient = (lightAmbient * lightAmbientStrength) * phongDiffuse;
	
	//calculating the Ambient lighting of the phong equation
    vec3 Diffuse = dotLN * lightValue * phongDiffuse;	

    
    //Blinn-Phong, comment the 2 lines below
    vec3 BlinnSpecular = phongSpecular * lightValue * pow(max(dot(N, normalize(L + E)), 0.f), phongShininess);
    vec3 color = phongEmitted + Ambient + Diffuse + BlinnSpecular;
    
	finalColor = vec4(color,0.0);
}
