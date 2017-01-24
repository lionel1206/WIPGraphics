////////////////////////////////////////////////////////////////////////
// The scene class contains all the parameters needed to define and
// draw the (really) simple scene, including:
//   * Geometry (in a display list)
//   * Light parameters
//   * Material properties
//   * Viewport size parameters
//   * Viewing transformation values
//   * others ...
//
// Some of these parameters are set when the scene is built, and
// others are set by the framework in response to user mouse/keyboard
// interactions.  All of them should be used to draw the scene.

#include "ambientLight.h"

class Scene
{
public:
    // Some user controllable parameters
    int mode;  // Communicated to the shaders as "mode".  Keys '0'-'9'
	int nSpheres;
	int diffFlag, specFlag, texFlag, shadowFlag, bumpFlag, reflFlag, fullonFlag;

    // Viewing transformation parameters;  Mouse buttons 1-3
    float front;
    float eyeSpin;
    float eyeTilt;
    float translatex;
    float translatey;
    float zoom;

	float cameraAngle;
	float cameraFi;
	float cameraRadius;
	glm::vec3 cameraLookAt;


    // Light position parameters;  Mouse buttons SHIFT 1-3
    float lightSpin;
    float lightTilt;
    float lightDist;
    
    // Viewport
    int width, height;
	
	// Objects in the scene
	unsigned int sphereVAO, sphereCount;
	unsigned int teapotVAO, teapotCount;
	unsigned int groundVAO, groundCount;
	unsigned int boxVAO;
	ambientLight mAmbientLight;

	// Shader programs
	ShaderProgram shaderFINAL;

};

void InitializeScene(Scene &scene);
void BuildScene(Scene &scene);
void DrawScene(Scene &scene);
