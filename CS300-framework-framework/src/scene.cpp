//////////////////////////////////////////////////////////////////////
// Defines and draws a scene.  There are two main procedures here:
//
// (1) void InitializeScene(Scene &scene): Is called before the main
// loop is entered, and is expected to setup all OpenGL objects needed
// for the rendering loop.
//
// (2) void DrawScene(Scene &scene): Is called each time the screen
// needs to be refreshed. (Which will be whenever the screen is
// expose/resized, anytime the preiodic animation clock ticks, or
// anytime there is user mouse/keyboard interaction.
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "shader.h"
#include "fbo.h"
#include "scene.h"
#include "models.h"
#include "globals.h"
#include "timer.h"

#include "math.h"
#include <fstream>
#include <stdlib.h>

#include "SOIL.h"

#include "GL\glew.h"
#include <GL/freeglut.h>
#include "glm\ext.hpp"
#include "glm\gtc\matrix_inverse.hpp"


////////////////////////////////////////////////////////////////////////
// This macro make sit easy to sprinkle checkd for OpenGL errors
// throught your code.  Most OpenGL calls can record errors, and a
// careful programmer will check the error status *often*, perhaps as
// often as after each OpenGL call.  At the very least, once per
// refresh will tell you if something is going wrong.
#define CHECKERROR {int err = glGetError(); if (err) { fprintf(stderr, "OpenGL error (at line %d): %s\n", __LINE__, gluErrorString(err)); exit(-1);} }


float ambientColor[3] = {0.5f, 0.5f, 0.5f};
float lightColor[3] = {0.5f, 0.5f, 0.5f};

float whiteColor[3] = {1.0f, 1.0f, 1.0f};
float blackColor[3] = {0.0f, 0.0f, 0.0f};
float diffuseColor[3] = {0.5f, 0.34f, 0.1f};
float specularColor[3] = {1.0f, 1.0f, 1.0f};
float shininess = 240;

const float PI = 3.14159f;
const float rad = PI/180.0f;
meshData boxMesh, sphereMesh, groundMesh, quadMesh;

void setUPGBuffer(Scene &scene)
{
	int width = (int)global::gWidth;
	int height = (int)global::gHeight;
	// bind FBO for G buffer
	glGenFramebuffers(1, &scene.gBufferData.gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, scene.gBufferData.gBuffer);
	CHECKERROR;
	// position texture buffer
	glGenTextures(1, &scene.gBufferData.gPositionTexture);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gPositionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene.gBufferData.gPositionTexture, 0);
	CHECKERROR;
	// Normal texture buffer
	glGenTextures(1, &scene.gBufferData.gNormalTexture);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, scene.gBufferData.gNormalTexture, 0);
	CHECKERROR;
	// albedo buffer
	glGenTextures(1, &scene.gBufferData.gAlbedoTexture);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gAlbedoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, scene.gBufferData.gAlbedoTexture, 0);
	CHECKERROR;
	// specular buffer
	glGenTextures(1, &scene.gBufferData.gSpecularTexture);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gSpecularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, scene.gBufferData.gSpecularTexture, 0);
	CHECKERROR;
	unsigned int colorAttachment[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, colorAttachment);
	CHECKERROR;
	// depth buffer
	glGenRenderbuffers(1, &scene.gBufferData.gDepthTexure);
	glBindRenderbuffer(GL_RENDERBUFFER, scene.gBufferData.gDepthTexure);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene.gBufferData.gDepthTexure);
	CHECKERROR;
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("FBO Error: %d\n", status);
	CHECKERROR;
}

unsigned int loadCube(const std::vector<const char*> &facePath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (unsigned int i = 0; i < facePath.size(); ++i)
	{
		int width, height, channel;
		unsigned char *image = SOIL_load_image(facePath[i], &width, &height, &channel, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

unsigned int loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
													   // Set our texture parameters
	CHECKERROR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECKERROR;
	return textureID;
}

////////////////////////////////////////////////////////////////////////
// InitializeScene is called once during setup to create all the
// textures, model VAOs, render target FBOs, and shader programs as
// well as a number of other parameters.
void InitializeScene(Scene &scene)
{
	CHECKERROR;

	scene.dirShadowMap.shadowMapWidth = 1280;
	scene.dirShadowMap.shadowMapHeight = 720;
	scene.dirShadowMap.lightFrustrumWidth = 100.f;
	scene.dirShadowMap.lightFrustrumHeight = 100.f;
	scene.dirShadowMap.shadowDistance = 200.f;
	scene.dirShadowMap.shadowDepthMap.CreateFBO(scene.dirShadowMap.shadowMapWidth, scene.dirShadowMap.shadowMapHeight);

	scene.gEditorCamera.initialize(glm::vec3(0,50,-100),
									 glm::vec3(0, 0, 0));
    
    // Enable OpenGL depth-testing
    glEnable(GL_DEPTH_TEST);

    // Create the scene models and textures
	if (!loadModelFromFile("assets/model/ground.json", groundMesh))
		std::cout << "Unable to load assets/model/ground.json" << std::endl;
	scene.groundVAO = createVAO(groundMesh);

	if (!loadModelFromFile("assets/model/cube.json", boxMesh))
		std::cout << "Unable to load assets/model/cube.json" << std::endl;
	scene.boxVAO = createVAO(boxMesh);

	if (!loadModelFromFile("assets/model/sphere.json", sphereMesh))
		std::cout << "Unable to load assets/model/sphere.json" << std::endl;
	scene.sphereVAO = createVAO(sphereMesh);

	scene.quad = createQuad(scene.quadCount);

	scene.groundTexture = loadTexture("assets/texture/ground_diffuse.png");
	scene.groundSpecular = loadTexture("assets/texture/ground_specular.png");
	scene.boxTexture = loadTexture("assets/texture/crate_diffuse.png");
	scene.boxSpecular= loadTexture("assets/texture/crate_specular.png");

	std::vector<const char*> skyBoxTexturePaths = { "assets/texture/skybox_right.tga", "assets/texture/skybox_left.tga",
		"assets/texture/skybox_up.tga", "assets/texture/skybox_down.tga",
		"assets/texture/skybox_back.tga", "assets/texture/skybox_front.tga", };
	scene.skyBoxTexture = loadCube(skyBoxTexturePaths);

	// initialize light data

	scene.mAmbientLight.setAmbientColor(glm::vec3(1.f, 1.f, 1.f));
	scene.mAmbientLight.setAmbientStrength(0.2f);
	scene.ambientLightParameters.ambientLightColor = scene.mAmbientLight.getAmbientColor();
	scene.ambientLightParameters.ambientLightStrength = scene.mAmbientLight.getAmbientStrength();

	glm::vec3 ptLightPosition[] = { glm::vec3(100, 85, -50), glm::vec3(0, 85, 100), glm::vec3(-100, 85, -50) };
	glm::vec4 ptLightAttenuation[] = { glm::vec4(325, 1.0, 0.014, 0.0007), glm::vec4(600, 1.0, 0.007, 0.0002), glm::vec4(160, 1.0, 0.027, 0.0028) };
	glm::vec3 ptLightColor[] = { glm::vec3(0,1,0), glm::vec3(1,0,0) , glm::vec3(0,0,1) };
	int MAX_POINT_LIGHT = 3;
	for (int i = 0; i < MAX_POINT_LIGHT; ++i)
	{
		pointLight ptLight = pointLight(ptLightPosition[i], scene.boxVAO, boxMesh.faces.size());
		ptLight.setLightColor(ptLightColor[i]);
		ptLight.setSpecularColor(ptLightColor[i]);
		ptLight.setLightIndex(i);
		ptLight.setAttenuationParameters(ptLightAttenuation[1].x, ptLightAttenuation[1].y, 
									     ptLightAttenuation[1].z, ptLightAttenuation[1].w);

		pointLightParam ptLightParam;
		ptLightParam.pointLightPosition = ptLightPosition[i];
		ptLightParam.pointLightDiffuse = ptLightColor[i];
		ptLightParam.pointLightSpecular = ptLightColor[i];
		ptLightParam.pointLightAttenuationDistance = ptLightAttenuation[1].x;
		ptLightParam.pointLightAttenuationConstanst = ptLightAttenuation[1].y;
		ptLightParam.pointLightAttenuationLinear = ptLightAttenuation[1].z;
		ptLightParam.pointLightAttenuationQuadratic = ptLightAttenuation[1].w;
		scene.pointLightContainer.push_back(ptLight);
		scene.pointLightParameters.push_back(ptLightParam);
	}

	directionalLight dirLight = directionalLight(glm::vec3(1, 0, 0));
	directionalLightParam dirLightParam;
	dirLightParam.directionLightDiffuse = dirLight.getDiffuseColor();
	dirLightParam.directionLightDir = dirLight.getLightDirection();
	dirLightParam.directionLightSpecular = dirLight.getSpecularColor();
	scene.directionalLightContainer.push_back(dirLight);
	scene.directionalLightParameters.push_back(dirLightParam);
	
	scene.mLightManager = lightManager(scene.pointLightContainer, scene.directionalLightContainer);

    // initialize and load shaders
	scene.shaderFINAL.CreateProgram();
	scene.shaderFINAL.CreateShader("shaders/phong_Color.vert", GL_VERTEX_SHADER);
	scene.shaderFINAL.CreateShader("shaders/phong_Color.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(scene.shaderFINAL.getProgram(), 0, "vertex");
	glBindAttribLocation(scene.shaderFINAL.getProgram(), 1, "vertexNormal");
	glBindAttribLocation(scene.shaderFINAL.getProgram(), 2, "vertexTexture");
	glBindAttribLocation(scene.shaderFINAL.getProgram(), 3, "vertexTangent");
	scene.shaderFINAL.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::BLINN_PHONG][global::eObjectMaterialType::COLOR] = scene.shaderFINAL;

	// Create the FINAL shader program from source code files.
	ShaderProgram textureShader;
	textureShader.CreateProgram();
	textureShader.CreateShader("shaders/phong_Texture.vert", GL_VERTEX_SHADER);
	textureShader.CreateShader("shaders/phong_Texture.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(textureShader.getProgram(), 0, "vertex");
	glBindAttribLocation(textureShader.getProgram(), 1, "vertexNormal");
	glBindAttribLocation(textureShader.getProgram(), 2, "vertexTexture");
	glBindAttribLocation(textureShader.getProgram(), 3, "vertexTangent");
	textureShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::BLINN_PHONG][global::eObjectMaterialType::TEXTURE] = textureShader;

	ShaderProgram textureSpecularShader;
	textureSpecularShader.CreateProgram(); 
	textureSpecularShader.CreateShader("shaders/phong_Texture_Specular.vert", GL_VERTEX_SHADER);
	textureSpecularShader.CreateShader("shaders/phong_Texture_Specular.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(textureSpecularShader.getProgram(), 0, "vertex");
	glBindAttribLocation(textureSpecularShader.getProgram(), 1, "vertexNormal");
	glBindAttribLocation(textureSpecularShader.getProgram(), 2, "vertexTexture");
	textureSpecularShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::BLINN_PHONG][global::eObjectMaterialType::TEXTURE_SPECULAR] = textureSpecularShader;

	ShaderProgram lightShader;
	lightShader.CreateProgram();
	lightShader.CreateShader("shaders/drawLight.vert", GL_VERTEX_SHADER);
	lightShader.CreateShader("shaders/drawLight.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(textureShader.getProgram(), 0, "vertex");
	lightShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::LIGHT_COLOR] = lightShader;

	ShaderProgram skyboxShader;
	skyboxShader.CreateProgram();
	skyboxShader.CreateShader("shaders/skybox.vert", GL_VERTEX_SHADER);
	skyboxShader.CreateShader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(skyboxShader.getProgram(), 0, "vertex");
	skyboxShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::TEXTURE_SKYBOX] = skyboxShader;

	ShaderProgram shadowDepthShader;
	shadowDepthShader.CreateProgram();
	shadowDepthShader.CreateShader("shaders/shadowDepth.vert", GL_VERTEX_SHADER);
	shadowDepthShader.CreateShader("shaders/shadowDepth.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(shadowDepthShader.getProgram(), 0, "vertex");
	shadowDepthShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::BLINN_PHONG][global::eObjectMaterialType::SHADOW_DEPTH] = shadowDepthShader;

	ShaderProgram quadRenderShader;
	quadRenderShader.CreateProgram();
	quadRenderShader.CreateShader("shaders/drawQuad.vert", GL_VERTEX_SHADER);
	quadRenderShader.CreateShader("shaders/drawQuad.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(quadRenderShader.getProgram(), 0, "vertex");
	glBindAttribLocation(quadRenderShader.getProgram(), 1, "vertexTexture");
	quadRenderShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::TEXTURE_QUAD] = quadRenderShader;

	ShaderProgram deferredGBufferShader;
	deferredGBufferShader.CreateProgram();
	deferredGBufferShader.CreateShader("shaders/deferred_gBuffer.vert", GL_VERTEX_SHADER);
	deferredGBufferShader.CreateShader("shaders/deferred_gBuffer.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(deferredGBufferShader.getProgram(), 0, "vertex");
	glBindAttribLocation(deferredGBufferShader.getProgram(), 1, "vertexNormal");
	glBindAttribLocation(deferredGBufferShader.getProgram(), 2, "vertexTexture");
	deferredGBufferShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::DEFERRED_BLINN_PHONG][global::eObjectMaterialType::DEFERRED_GBUFFER] = deferredGBufferShader;

	ShaderProgram deferredGBufferGammaShader;
	deferredGBufferGammaShader.CreateProgram();
	deferredGBufferGammaShader.CreateShader("shaders/deferred_gBuffer.vert", GL_VERTEX_SHADER);
	deferredGBufferGammaShader.CreateShader("shaders/deferred_gBufferGamma.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(deferredGBufferGammaShader.getProgram(), 0, "vertex");
	glBindAttribLocation(deferredGBufferGammaShader.getProgram(), 1, "vertexNormal");
	glBindAttribLocation(deferredGBufferGammaShader.getProgram(), 2, "vertexTexture");
	deferredGBufferGammaShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::DEFERRED_BLINN_PHONG][global::eObjectMaterialType::DEFERRED_GBUFFER_GAMMA] = deferredGBufferGammaShader;

	ShaderProgram deferredLightPassShader;
	deferredLightPassShader.CreateProgram();
	deferredLightPassShader.CreateShader("shaders/deferred_lightPass.vert", GL_VERTEX_SHADER);
	deferredLightPassShader.CreateShader("shaders/deferred_lightPass.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(deferredLightPassShader.getProgram(), 0, "vertex");
	glBindAttribLocation(deferredLightPassShader.getProgram(), 1, "vertexTexture");
	deferredLightPassShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::DEFERRED_BLINN_PHONG][global::eObjectMaterialType::DEFERRED_LIGHTING_PASS] = deferredLightPassShader;

	ShaderProgram deferredLightPassGammaShader;
	deferredLightPassGammaShader.CreateProgram();
	deferredLightPassGammaShader.CreateShader("shaders/deferred_lightPass.vert", GL_VERTEX_SHADER);
	deferredLightPassGammaShader.CreateShader("shaders/deferred_lightPassGamma.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(deferredLightPassGammaShader.getProgram(), 0, "vertex");
	glBindAttribLocation(deferredLightPassGammaShader.getProgram(), 1, "vertexTexture");
	deferredLightPassGammaShader.LinkProgram();
	CHECKERROR;
	scene.shaderLibrary[global::eLightingType::DEFERRED_BLINN_PHONG][global::eObjectMaterialType::DEFERRED_LIGHTING_PASS_GAMMA] = deferredLightPassGammaShader;

	scene.fovDeg = 45.f;
	scene.nearplane = 0.1f;
	scene.farplane = 20000.f;
	scene.perspectiveMtx = glm::perspective(scene.fovDeg, global::gWidth / global::gHeight,
											scene.nearplane, scene.farplane);


	// initialize and crate scene objects
	graphicObject groundObject(glm::vec3(0.f, 0.f, 0.f), glm::vec3(), glm::vec3(250,1,250), scene.groundVAO, groundMesh.faces.size());
	groundObject.setTextureMap(scene.groundTexture); 
	groundObject.setSpecularMap(scene.groundSpecular);
	scene.graphicsObjectContainer.push_back(groundObject);

	glm::vec3 boxPosition[27] = { glm::vec3(-35, 15, -35), glm::vec3(0, 15, -35), glm::vec3(35, 15, -35),
								 glm::vec3(-35, 15, 0), glm::vec3(0, 15, 0), glm::vec3(35, 15, 0),
		                         glm::vec3(-35, 15, 35), glm::vec3(0, 15, 35), glm::vec3(35, 15, 35),

		                         glm::vec3(-35, 50, -35), glm::vec3(0, 50, -35), glm::vec3(35, 50, -35),
		                         glm::vec3(-35, 50, 0), glm::vec3(0, 50, 0), glm::vec3(35, 50, 0),
		                         glm::vec3(-35, 50, 35), glm::vec3(0, 50, 35), glm::vec3(35, 50, 35),

		                         glm::vec3(-35, 85, -35), glm::vec3(0, 85, -35), glm::vec3(35, 85, -35),
		                         glm::vec3(-35, 85, 0), glm::vec3(0, 85, 0), glm::vec3(35, 85, 0),
		                         glm::vec3(-35, 85, 35), glm::vec3(0, 85, 35), glm::vec3(35, 85, 35) };
	glm::vec3 boxRotation[5] = { glm::vec3(0, 0, 0), glm::vec3(20, 10, 40), glm::vec3(45, 0, 45), glm::vec3(45, 20, 10), glm::vec3(20, 20, 0) };
	glm::vec3 boxScale = glm::vec3(20, 20, 20);
	int MAX_BOX = 9;
	for (int i = 0; i < MAX_BOX; ++i)
	{
		graphicObject boxObject(boxPosition[i], glm::vec3(), boxScale, scene.boxVAO, boxMesh.faces.size());
		boxObject.setTextureMap(scene.boxTexture);
		boxObject.setSpecularMap(scene.boxSpecular);
		scene.graphicsObjectContainer.push_back(boxObject);
	}

	setUPGBuffer(scene);
	CHECKERROR;
}
////////////////////////////////////////////////////////////////////////

void renderGeometry(Scene &scene, unsigned int shader)
{
	for (unsigned int i = 0; i < scene.graphicsObjectContainer.size(); ++i)
	{
		scene.mAmbientLight.updateLightParameter(shader);
		scene.mLightManager.passDataToShader(shader);
		scene.graphicsObjectContainer[i].draw(shader);
		CHECKERROR;
	}
}

void renderLightingPass(Scene &scene)
{
	auto materialType = global::eObjectMaterialType::DEFERRED_LIGHTING_PASS;
	if (scene.enableGammaCorrection)
	{
		materialType = global::eObjectMaterialType::DEFERRED_LIGHTING_PASS_GAMMA;
	}
	ShaderProgram deferredLightPassShader = scene.shaderLibrary[global::eLightingType::DEFERRED_BLINN_PHONG][materialType];
	deferredLightPassShader.Use();
	unsigned int shader = deferredLightPassShader.getProgram();
	scene.mAmbientLight.updateLightParameter(shader);
	scene.mLightManager.passDataToShader(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gPositionTexture);
	int loc = glGetUniformLocation(shader, "gPositionTexture");
	glUniform1i(loc, 0);
	CHECKERROR;
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gNormalTexture);
	loc = glGetUniformLocation(shader, "gNormalTexture");
	glUniform1i(loc, 1);
	CHECKERROR;
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gAlbedoTexture);
	loc = glGetUniformLocation(shader, "gAlbedoTexture");
	glUniform1i(loc, 2);
	CHECKERROR;
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gSpecularTexture);
	loc = glGetUniformLocation(shader, "gSpecularTexture");
	glUniform1i(loc, 3);
	CHECKERROR;
	loc = glGetUniformLocation(shader, "cameraPos");
	glUniform3fv(loc, 1, glm::value_ptr(scene.gEditorCamera.getPosition()));
	glBindVertexArray(scene.quad);
	glDrawElements(GL_TRIANGLES, scene.quadCount, GL_UNSIGNED_INT, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
	deferredLightPassShader.Unuse();
}

void drawGBuffer(Scene &scene)
{
	ShaderProgram quadShader = scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::TEXTURE_QUAD];
	quadShader.Use();

	// draw G buffer position
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gPositionTexture);
		int loc = glGetUniformLocation(quadShader.getProgram(), "texture");
		glUniform1i(loc, 0);
		glm::mat4 scale = glm::scale(glm::vec3(0.25, 0.25, 0.25));
		glm::mat4 translate = glm::translate(glm::vec3(0.75, 0.75, 0));
		glm::mat4 transform = translate * scale;
		loc = glGetUniformLocation(quadShader.getProgram(), "transform");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(scene.quad);
		glDrawElements(GL_TRIANGLES, scene.quadCount, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
	}

	// draw G Buffer Normals
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gNormalTexture);
		int loc = glGetUniformLocation(quadShader.getProgram(), "texture");
		glUniform1i(loc, 0);
		glm::mat4 scale = glm::scale(glm::vec3(0.25, 0.25, 0.25));
		glm::mat4 translate = glm::translate(glm::vec3(0.75, 0.25, 0));
		glm::mat4 transform = translate * scale;
		loc = glGetUniformLocation(quadShader.getProgram(), "transform");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(scene.quad);
		glDrawElements(GL_TRIANGLES, scene.quadCount, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
	}

	// draw G Buffer Albedo
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gAlbedoTexture);
		int loc = glGetUniformLocation(quadShader.getProgram(), "texture");
		glUniform1i(loc, 0);
		glm::mat4 scale = glm::scale(glm::vec3(0.25, 0.25, 0.25));
		glm::mat4 translate = glm::translate(glm::vec3(0.75, -0.25, 0));
		glm::mat4 transform = translate * scale;
		loc = glGetUniformLocation(quadShader.getProgram(), "transform");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(scene.quad);
		glDrawElements(GL_TRIANGLES, scene.quadCount, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
	}

	// draw G Buffer specular
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene.gBufferData.gSpecularTexture);
		int loc = glGetUniformLocation(quadShader.getProgram(), "texture");
		glUniform1i(loc, 0);
		glm::mat4 scale = glm::scale(glm::vec3(0.25, 0.25, 0.25));
		glm::mat4 translate = glm::translate(glm::vec3(0.75, -0.75, 0));
		glm::mat4 transform = translate * scale;
		loc = glGetUniformLocation(quadShader.getProgram(), "transform");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(scene.quad);
		glDrawElements(GL_TRIANGLES, scene.quadCount, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
	}

	quadShader.Unuse();
	CHECKERROR;
}

////////////////////////////////////////////////////////////////////////
// Procedure DrawScene is called whenever the scene needs to be drawn.
void DrawScene(Scene &scene)
{    
    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	CHECKERROR;
	scene.gEditorCamera.update();
	glm::mat4x4 viewMtx = scene.gEditorCamera.getViewMtx();
	
	// Calculate the inverse of the viewing matrix and send to the shader
	glm::mat4x4 inverseViewMtx = glm::affineInverse(viewMtx);

	scene.mLightManager.updateLightParameters(scene.pointLightParameters, scene.directionalLightParameters);
	scene.mAmbientLight.setAmbientColor(scene.ambientLightParameters.ambientLightColor);
	scene.mAmbientLight.setAmbientStrength(scene.ambientLightParameters.ambientLightStrength);
	CHECKERROR;
	// render to G BUFFER
	{
		glBindFramebuffer(GL_FRAMEBUFFER, scene.gBufferData.gBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		auto lightType = global::eLightingType::DEFERRED_BLINN_PHONG;
		auto modelMaterial = global::eObjectMaterialType::DEFERRED_GBUFFER;
		if (scene.enableGammaCorrection)
		{
			modelMaterial = global::eObjectMaterialType::DEFERRED_GBUFFER_GAMMA;
		}
		ShaderProgram currentShader = scene.shaderLibrary[lightType][modelMaterial];
		currentShader.Use();
		int loc = glGetUniformLocation(currentShader.getProgram(), "ProjectionMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(scene.perspectiveMtx));
		loc = glGetUniformLocation(currentShader.getProgram(), "ViewMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMtx));
		renderGeometry(scene, currentShader.getProgram());
		currentShader.Unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	CHECKERROR;
	// deferred lighting pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderLightingPass(scene);
	CHECKERROR;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, scene.gBufferData.gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, scene.width, scene.height, 0, 0, scene.width, scene.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECKERROR;

	// render the physical light objects in the scene
	{
		ShaderProgram lightShader = scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::LIGHT_COLOR];
		lightShader.Use();
		int loc = glGetUniformLocation(lightShader.getProgram(), "ProjectionMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(scene.perspectiveMtx));
		loc = glGetUniformLocation(lightShader.getProgram(), "ViewMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMtx));
		scene.mLightManager.draw(lightShader.getProgram());
		lightShader.Unuse();
	}
	CHECKERROR;

	// render the G buffer
	if(scene.showGBuffer)
	{
		drawGBuffer(scene);
	}
	
	// skybox render
	{
		glDepthFunc(GL_LEQUAL);
		ShaderProgram skyboxProgram = scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::TEXTURE_SKYBOX];
		skyboxProgram.Use();
		int loc = glGetUniformLocation(skyboxProgram.getProgram(), "ProjectionMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(scene.perspectiveMtx));
		glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMtx));
		loc = glGetUniformLocation(skyboxProgram.getProgram(), "ViewMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(skyboxView));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, scene.skyBoxTexture);
		loc = glGetUniformLocation(skyboxProgram.getProgram(), "skybox");
		glUniform1i(loc, 0);
		glBindVertexArray(scene.boxVAO);
		glDrawElements(GL_TRIANGLES, boxMesh.faces.size(), GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
		skyboxProgram.Unuse();
		glDepthFunc(GL_LESS);
	}
	CHECKERROR;
}
