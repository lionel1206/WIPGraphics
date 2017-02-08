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
#include "pointLight.h"
#include "directionalLight.h"
#include "camera.h"
#include "graphicObject.h"
#include "lightManager.h"
#include "fbo.h"
#include <vector>

struct directionalShadowMapParam
{
	int shadowMapWidth, shadowMapHeight;
	float shadowDistance;
	float lightFrustrumWidth, lightFrustrumHeight;
	FBO shadowDepthMap;
};

struct dsGBufferParam
{
	unsigned int gBuffer;
	unsigned int gPositionTexture;
	unsigned int gNormalTexture;
	unsigned int gAlbedoTexture;
	unsigned int gSpecularTexture;
	unsigned int gDepthTexure;
};

class Scene
{
public:

    // Viewing transformation parameters;  Mouse buttons 1-3
    float front;
    float eyeSpin;
    float eyeTilt;
    float translatex;
    float translatey;
    float zoom;


    // Light position parameters;  Mouse buttons SHIFT 1-3
    float lightSpin;
    float lightTilt;
    float lightDist;
    
    // Viewport
    int width, height;
	
	// Objects in the scene
	unsigned int sphereVAO, sphereCount;
	unsigned int teapotVAO, teapotCount;
	unsigned int groundVAO, groundTexture, groundSpecular;
	unsigned int boxVAO, boxTexture, boxSpecular;
	unsigned int planeVAO, planceTexture;
	unsigned int skyBoxTexture;
	
	camera gEditorCamera;
	glm::mat4 perspectiveMtx;
	ambientLight mAmbientLight;
	pointLight ptLight;
	directionalLight dirLight;
	float fovDeg;
	float nearplane, farplane;

	ShaderProgram shaderLibrary[global::eLightingType::MAX_LIGHTING_COUNT][global::eObjectMaterialType::MAX_MATERIAL_COUNT];
	// Shader programs
	ShaderProgram shaderFINAL;
	std::vector<graphicObject> graphicsObjectContainer;
	std::vector<pointLight> pointLightContainer;
	pointLightParamContainter pointLightParameters;
	std::vector<directionalLight> directionalLightContainer;
	directionLightParamContainter directionalLightParameters;
	ambientLightParam ambientLightParameters;
	lightManager mLightManager;
	directionalShadowMapParam dirShadowMap;
	bool showShadowDepthMap = true;
	unsigned int quad, quadCount;
	graphicObject quadObject;
	dsGBufferParam gBufferData;
	bool showGBuffer = true;
	bool enableGammaCorrection = true;
};
void renderGeometry(Scene &scene, unsigned int shader);
void gatherShadowInfo(Scene &scene);
void setUPGBuffer(Scene &scene);
void renderLightingPass(Scene &scene);
void drawGBuffer(Scene &scene);
unsigned int loadTexture(const char* path);
unsigned int loadCube(const std::vector<const char*> &facePath);
void InitializeScene(Scene &scene);
void BuildScene(Scene &scene);
void DrawScene(Scene &scene);
