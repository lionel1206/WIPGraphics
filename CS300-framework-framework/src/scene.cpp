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
meshData boxMesh, sphereMesh, groundMesh;

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

	scene.shadowMapWidth = 1280;
	scene.shadowMapHeight = 720;

	scene.gEditorCamera.initialize(glm::vec3(0,50,-100),
									 glm::vec3(0, 0, 0));

	scene.mAmbientLight.setAmbientColor(glm::vec3(1.f, 1.f, 1.f));
	scene.mAmbientLight.setAmbientStrength(0.2f);
	scene.ambientLightParameters.ambientLightColor = scene.mAmbientLight.getAmbientColor();
	scene.ambientLightParameters.ambientLightStrength = scene.mAmbientLight.getAmbientStrength();
    
    // Enable OpenGL depth-testing
    glEnable(GL_DEPTH_TEST);

    // Create the scene models
	if (!loadModelFromFile("assets/model/ground.json", groundMesh))
		std::cout << "Unable to load assets/model/ground.json" << std::endl;
	scene.groundVAO = createVAO(groundMesh);

	if (!loadModelFromFile("assets/model/cube.json", boxMesh))
		std::cout << "Unable to load assets/model/cube.json" << std::endl;
	scene.boxVAO = createVAO(boxMesh);

	

	glm::vec3 ptLightPosition[] = { glm::vec3(100, 85, -50), glm::vec3(0, 85, 50), glm::vec3(-100, 85, -50) };
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

	scene.groundTexture = loadTexture("assets/texture/floor_diffuse.png");
	scene.boxTexture = loadTexture("assets/texture/crate_diffuse.png");

	if (!loadModelFromFile("assets/model/sphere.json", sphereMesh))
		std::cout << "Unable to load assets/model/sphere.json" << std::endl;
	scene.sphereVAO = createVAO(sphereMesh);

	std::vector<const char*> skyBoxTexturePaths = { "assets/texture/skybox_right.tga", "assets/texture/skybox_left.tga",
		                                            "assets/texture/skybox_up.tga", "assets/texture/skybox_down.tga", 
		                                            "assets/texture/skybox_back.tga", "assets/texture/skybox_front.tga", };
	scene.skyBoxTexture = loadCube(skyBoxTexturePaths);

    // Create the FINAL shader program from source code files.
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

	scene.fovDeg = 45.f;
	scene.nearplane = 0.1f;
	scene.farplane = 20000.f;
	scene.perspectiveMtx = glm::perspective(scene.fovDeg, global::gWidth / global::gHeight,
											scene.nearplane, scene.farplane);

	graphicObject groundObject(glm::vec3(0.f, 0.f, 0.f), glm::vec3(), glm::vec3(250,1,250), scene.groundVAO, groundMesh.faces.size());
	//groundObject.setColor(glm::vec3(diffuseColor[0], diffuseColor[1], diffuseColor[2]));
	groundObject.setTextureMap(scene.groundTexture); 
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
		scene.graphicsObjectContainer.push_back(boxObject);
	}
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Procedure DrawScene is called whenever the scene needs to be drawn.
void DrawScene(Scene &scene)
{    
    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	scene.gEditorCamera.update();
	glm::mat4x4 viewMtx = scene.gEditorCamera.getViewMtx();
	
	// Calculate the inverse of the viewing matrix and send to the shader
	glm::mat4x4 inverseViewMtx = glm::affineInverse(viewMtx);

	glDepthMask(GL_FALSE);
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
	glDepthMask(GL_TRUE);

	scene.mLightManager.updateLightParameters(scene.pointLightParameters, scene.directionalLightParameters);
	scene.mAmbientLight.setAmbientColor(scene.ambientLightParameters.ambientLightColor);
	scene.mAmbientLight.setAmbientStrength(scene.ambientLightParameters.ambientLightStrength);

	for (unsigned int i = 0; i < scene.graphicsObjectContainer.size(); ++i)
	{
		auto lightType = global::eLightingType::BLINN_PHONG;
		auto modelMaterial = scene.graphicsObjectContainer[0].getMaterialType();
		ShaderProgram currentShader = scene.shaderLibrary[lightType][modelMaterial];

		currentShader.Use();

		loc = glGetUniformLocation(currentShader.getProgram(), "ProjectionMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(scene.perspectiveMtx));

		loc = glGetUniformLocation(currentShader.getProgram(), "ViewMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMtx));
		CHECKERROR;

		loc = glGetUniformLocation(currentShader.getProgram(), "cameraPos");
		glUniform3fv(loc, 1, glm::value_ptr(scene.gEditorCamera.getPosition()));

		loc = glGetUniformLocation(currentShader.getProgram(), "ViewInverse");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(inverseViewMtx));
		CHECKERROR;

		
		scene.mAmbientLight.updateLightParameter(currentShader.getProgram());
		scene.mLightManager.pushLightInfoToGPU(currentShader.getProgram());
		scene.graphicsObjectContainer[i].draw(currentShader.getProgram());

		currentShader.Unuse();
		CHECKERROR;
	}

	// draw lights
	ShaderProgram lightShader = scene.shaderLibrary[global::eLightingType::NO_LIGHTING][global::eObjectMaterialType::LIGHT_COLOR];
	lightShader.Use();
	loc = glGetUniformLocation(lightShader.getProgram(), "ProjectionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(scene.perspectiveMtx));
	loc = glGetUniformLocation(lightShader.getProgram(), "ViewMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMtx));
	CHECKERROR;
	scene.mLightManager.drawLight(lightShader.getProgram());
	lightShader.Unuse();
}
