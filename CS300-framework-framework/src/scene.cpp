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

#include "math.h"
#include <fstream>
#include <stdlib.h>

#include <glload/gl_3_3.h>
#include <glload/gll.hpp>
#include <GL/freeglut.h>
#include "glimg/glimg.h"
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
float lightColor[3] = {0.8f, 0.8f, 0.8f};

float whiteColor[3] = {1.0f, 1.0f, 1.0f};
float blackColor[3] = {0.0f, 0.0f, 0.0f};
float diffuseColor[3] = {0.5f, 0.34f, 0.1f};
float specularColor[3] = {1.0f, 1.0f, 1.0f};
int shininess = 120;

const float PI = 3.14159f;
const float rad = PI/180.0f;
meshData boxMesh, sphereMesh, groundMesh;
////////////////////////////////////////////////////////////////////////
// A small function to provide a more friendly method of defining
// colors.  The parameters are hue (0..1: fraction of distance around
// the color wheel; red at 0, green at 1/3, blue at 2/3), saturation
// (0..1: achromatic to saturated), and value (0..1: brightness).
#define ret(r,g,b) {c[0]=r; c[1]=g; c[2]=b; }
void HSV2RGB(const float h, const float s, const float v, float *c)
{
	if (s == 0.0)
		ret(v,v,v)
	else {
		int i = (int)(h*6.0);
		float f = (h*6.0f) - i;
		float p = v*(1.0f - s);
		float q = v*(1.0f - s*f);
		float t = v*(1.0f - s*(1.0f-f));
		if (i%6 == 0)
			ret(v,t,p)
		else if (i == 1)
			ret(q,v,p)
		else if (i == 2)
			ret(p,v,t)
		else if (i == 3)
			ret(p,q,v)
		else if (i == 4)
			ret(t,p,v)
		else if (i == 5)
			ret(v,p,q) }
}

////////////////////////////////////////////////////////////////////////
// InitializeScene is called once during setup to create all the
// textures, model VAOs, render target FBOs, and shader programs as
// well as a number of other parameters.
void InitializeScene(Scene &scene)
{
	CHECKERROR;
    scene.mode = 0;
    scene.nSpheres = 16;

	scene.diffFlag=1;
	scene.specFlag=2;
	scene.texFlag=4;
	scene.shadowFlag=8;
	scene.bumpFlag=16;
	scene.reflFlag=32;
	scene.fullonFlag=64;

    // Set the initial viewing transformation parameters
    scene.front = 1.0;
    scene.eyeSpin = -150.0;
    scene.eyeTilt = -60.0;
    scene.translatex = 0.0;
    scene.translatey = -1.0;
    scene.zoom = 30.0;

	scene.cameraAngle = 45.f;
	scene.cameraFi = 45.f;
	scene.cameraRadius = 50.f;
	scene.cameraLookAt = glm::vec3(0, 0, 0);
	global::gEditorCamera.initialize(scene.cameraAngle,
		                             scene.cameraFi,
		                             scene.cameraRadius,
		                             scene.cameraLookAt);

    // Set the initial light position parammeters
    scene.lightSpin = -90;
    scene.lightTilt = -60.0;
    scene.lightDist = 60.0;
    
    // Enable OpenGL depth-testing
    glEnable(GL_DEPTH_TEST);

    // Create the scene models
	if (!loadModelFromFile("assets/model/ground.json", groundMesh))
		std::cout << "Unable to load assets/model/ground.json" << std::endl;
	scene.groundVAO = createVAO(groundMesh);

	if (!loadModelFromFile("assets/model/cube.json", boxMesh))
		std::cout << "Unable to load assets/model/cube.json" << std::endl;
	scene.boxVAO = createVAO(boxMesh);

	if (!loadModelFromFile("assets/model/sphere.json", sphereMesh))
		std::cout << "Unable to load assets/model/sphere.json" << std::endl;
	scene.sphereVAO = createVAO(sphereMesh);

    // Create the FINAL shader program from source code files.
    scene.shaderFINAL.CreateProgram();
    scene.shaderFINAL.CreateShader("shaders\\final.vert", GL_VERTEX_SHADER);
    scene.shaderFINAL.CreateShader("shaders\\phong.vert", GL_VERTEX_SHADER);

    scene.shaderFINAL.CreateShader("shaders\\final.frag", GL_FRAGMENT_SHADER);
    scene.shaderFINAL.CreateShader("shaders\\phong.frag", GL_FRAGMENT_SHADER);
	glBindAttribLocation(scene.shaderFINAL.program, 0, "vertex");
	glBindAttribLocation(scene.shaderFINAL.program, 1, "vertexNormal");
	glBindAttribLocation(scene.shaderFINAL.program, 2, "vertexTexture");
	glBindAttribLocation(scene.shaderFINAL.program, 3, "vertexTangent");
    scene.shaderFINAL.LinkProgram();

	CHECKERROR;
}



////////////////////////////////////////////////////////////////////////
// Called regularly to update the rotation of the surrounding sphere
// environment.  Set to rotate once every two minutes.
float atime = 0.0;
void animate(int value)
{
	atime = 360.0*glutGet(GLUT_ELAPSED_TIME)/120000;
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////
// Procedure DrawScene is called whenever the scene needs to be drawn.
void DrawScene(Scene &scene)
{    
	CHECKERROR;

	glm::mat4 I(1.0f);
	glm::mat4x4 RA = glm::rotate(atime, 0.0f, 0.0f, 1.0f);

	glutTimerFunc(100, animate, 1);

    // Calculate the light's position from values controlled by the
    // user, and tell OpenGL where it is located.
    float lPos[4] = {
        scene.lightDist*cos(scene.lightSpin*rad)*sin(scene.lightTilt*rad),
        scene.lightDist*sin(scene.lightSpin*rad)*sin(scene.lightTilt*rad),
        scene.lightDist*cos(scene.lightTilt*rad),
        1.0 };

    ///////////////////////////////////////////////////////////////////
    // Lighting pass: Draw the scene with lighting calculations.
    ///////////////////////////////////////////////////////////////////

    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	// Use the shader program
	scene.shaderFINAL.Use();

    float sx, sy;
    sy = 0.2f*scene.front;
	sx = sy * scene.width/scene.height;
	CHECKERROR;

	////////////////////////////////////////////////////////////////////////
	// Send Model, View and Perspective transformations to OpenGL
	////////////////////////////////////////////////////////////////////////
	
	// Create the perspective matrix and send to the shader
	glm::mat4x4 P = glm::frustum(-sx, sx, -sy, sy, scene.front, 1000.0f);
    int loc = glGetUniformLocation(scene.shaderFINAL.program, "ProjectionMatrix"); 
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(P));

	// Build a simple viewing matrix and send to the shader
	global::gEditorCamera.update();
	/*glm::mat4x4 V1 = glm::translate(scene.translatex, scene.translatey, -scene.zoom);
	glm::mat4x4 V2 = glm::rotate(V1, scene.eyeTilt, 1.0f, 0.0f, 0.0f);
	glm::mat4x4 V3 = glm::rotate(V2, scene.eyeSpin, 0.0f, 0.0f, 1.0f);*/
	glm::mat4x4 V3 = global::gEditorCamera.getViewMtx();
    loc = glGetUniformLocation(scene.shaderFINAL.program, "ViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(V3)); 
	
	// Calculate the inverse of the viewing matrix and send to the shader
	glm::mat4x4 INV = glm::affineInverse(V3);
	loc = glGetUniformLocation(scene.shaderFINAL.program, "ViewInverse");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(INV));

    // Initialize the modeling matrix and send to the shader
    loc = glGetUniformLocation(scene.shaderFINAL.program, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(I)); 
    // And its inverse
    loc = glGetUniformLocation(scene.shaderFINAL.program, "NormalMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(I)); 
	CHECKERROR;

	////////////////////////////////////////////////////////////////////////
	// Send lighting parameters to OpenGL
	////////////////////////////////////////////////////////////////////////
	
	// Inform the shader of the light's parameters
	loc = glGetUniformLocation(scene.shaderFINAL.program, "lightAmbient");
	glUniform3fv(loc, 1, ambientColor);
	loc = glGetUniformLocation(scene.shaderFINAL.program, "lightPos");
	glUniform3fv(loc, 1, lPos);
	loc = glGetUniformLocation(scene.shaderFINAL.program, "lightValue");
	glUniform3fv(loc, 1, lightColor);
	CHECKERROR;

	// Set a variable that is useful for debugging purposes.
    loc = glGetUniformLocation(scene.shaderFINAL.program, "mode");
    glUniform1i(loc, scene.mode);  
	CHECKERROR; 

	// Tell the shader of the screen width and height (for later use)
	loc = glGetUniformLocation(scene.shaderFINAL.program, "WIDTH");
    glUniform1i(loc, scene.width);  
	CHECKERROR; 

	loc = glGetUniformLocation(scene.shaderFINAL.program, "HEIGHT");
    glUniform1i(loc, scene.height);
	CHECKERROR;
	
	// Set a boring generic set of phong parameters for the following surface.
	loc = glGetUniformLocation(scene.shaderFINAL.program, "phongDiffuse");
	glUniform3fv(loc, 1, diffuseColor);  
	loc = glGetUniformLocation(scene.shaderFINAL.program, "phongSpecular");
	glUniform3fv(loc, 1, specularColor);  
	loc = glGetUniformLocation(scene.shaderFINAL.program, "phongShininess");
	glUniform1f(loc, shininess);  
	CHECKERROR;

	////////////////////////////////////////////////////////////////////////
	// Draw the scene's objects
	////////////////////////////////////////////////////////////////////////
	
	// Draw the ground plane
	float scale = 100.f;
	glm::mat4 scaleMtx = glm::scale(glm::vec3(scale, 1.f, scale));
	//glm::mat4 rotateMtx = glm::rotate(90.f, glm::vec3(1,0,0));
	glm::mat4 TranslateMtx = glm::translate(glm::vec3(0, 0, 0));
	glm::mat4x4 ModelMtx = TranslateMtx * scaleMtx;
	loc = glGetUniformLocation(scene.shaderFINAL.program, "ModelMatrix");
	glBindVertexArray(scene.groundVAO);
	glDrawElements(GL_TRIANGLES, groundMesh.faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	CHECKERROR;


	// draw box
	scale = 5.f;
	scaleMtx = glm::scale(glm::vec3(scale, scale, scale));
	TranslateMtx = glm::translate(glm::vec3(0, 0, 5));
	ModelMtx = TranslateMtx * scaleMtx;
	loc = glGetUniformLocation(scene.shaderFINAL.program, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMtx));

	int dloc = glGetUniformLocation(scene.shaderFINAL.program, "phongDiffuse");
	glm::vec3 boxDiffuse(1.f, 0.f, 0.f);
	glUniform3fv(dloc, 1, glm::value_ptr(boxDiffuse));

	glBindVertexArray(scene.boxVAO);
	glDrawElements(GL_TRIANGLES, boxMesh.faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	CHECKERROR;
	
	// A small white sphere (translated to the correct position) represents the light.
	loc = glGetUniformLocation(scene.shaderFINAL.program, "phongEmitted");
	glUniform3fv(loc, 1, whiteColor); // Light emits full-on white
	CHECKERROR;

	glm::mat4x4 T = glm::translate(lPos[0],lPos[1],lPos[2]); // Translation for light
    loc = glGetUniformLocation(scene.shaderFINAL.program, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(T)); 
	CHECKERROR;

	glBindVertexArray(scene.sphereVAO);
	glDrawElements(GL_TRIANGLES, sphereMesh.faces.size(), GL_UNSIGNED_INT, 0); // Draws light
	glBindVertexArray(0);
	CHECKERROR;

    loc = glGetUniformLocation(scene.shaderFINAL.program, "ModelMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(I)); // Reset light's translate
	CHECKERROR;

	loc = glGetUniformLocation(scene.shaderFINAL.program, "phongEmitted");
	glUniform3fv(loc, 1, blackColor); // Turns off emitted light
	CHECKERROR;

	// Done with shader program
	scene.shaderFINAL.Unuse();
	CHECKERROR;
    
}
