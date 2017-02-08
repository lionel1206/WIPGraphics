///////////////////////////////////////////////////////////////////////
// Provides the framework for graphics projects.  Most of this small
// file contains the GLUT calls needed to open a window and hook up
// various callbacks for mouse/keyboard interaction and screen resizes
// and redisplays.
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "shader.h"
#include "fbo.h"
#include "scene.h"

#ifdef _WIN32
    // Includes for Windows
    #include <windows.h>
    #include <cstdlib>
    #include <fstream>
    #include <limits>
    #include <crtdbg.h>
#define _CRT_SECURE_NO_WARNINGS
#else
    // Includes for Linux
    #include <algorithm>
    #define sprintf_s sprintf
    #include <fstream>
#endif

#include "GL\glew.h"
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "AntTweakBar.h"
#include <sstream>
#include "math.h"

#include "timer.h"
#include "globals.h"
Scene scene;

// Some globals used for mouse handling.
int mouseX, mouseY;
bool leftDown = false;
bool middleDown = false;
bool rightDown = false;
bool shifted;

////////////////////////////////////////////////////////////////////////
// Called by GLUT when the scene needs to be redrawn.
void ReDraw()
{
    DrawScene(scene);
	TwDraw();
    glutSwapBuffers();
	
}

void TimerUpdate(int time)
{
	global::timer::updateTimer(glutGet(GLUT_ELAPSED_TIME) / 1000.f);
	glutPostRedisplay();
	glutTimerFunc(1000.f / 60.f, TimerUpdate, 0);
}

////////////////////////////////////////////////////////////////////////
// Called by GLUT when the window size is changed.
void ReshapeWindow(int w, int h)
{
    if (w && h) {
        glViewport(0, 0, w, h); }
    scene.width = w;
    scene.height = h;
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////
// Called by GLut for keyboard actions.
void KeyboardInput(unsigned char key, int x, int y)
{
	if (!TwEventKeyboardGLUT(key, x, y))
	{
		switch (key) {

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			glutPostRedisplay();
			break;

		case 27:                    // Escape key
			exit(0);
		}
	}
    
}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse button changes state.
void MouseButton(int button, int state, int x, int y)
{
	if (!TwEventMouseButtonGLUT(button, state, x, y))
	{
		shifted = glutGetModifiers() && GLUT_ACTIVE_SHIFT;

		if (button == GLUT_LEFT_BUTTON)
			leftDown = (state == GLUT_DOWN);

		else if (button == GLUT_MIDDLE_BUTTON)
			middleDown = (state == GLUT_DOWN);

		else if (button == GLUT_RIGHT_BUTTON)
			rightDown = (state == GLUT_DOWN);

		else if (button % 8 == 3 && shifted)
			scene.lightDist = pow(scene.lightDist, 1.0f / 1.02f);

		else if (button % 8 == 3)
			scene.zoom = pow(scene.zoom, 1.0f / 1.02f);

		else if (button % 8 == 4 && shifted)
			scene.lightDist = pow(scene.lightDist, 1.02f);

		else if (button % 8 == 4)
			scene.zoom = pow(scene.zoom, 1.02f);

		mouseX = x;
		mouseY = y;

		//glutPostRedisplay();
	}
}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse moves (while a button is down)
void MouseMotion(int x, int y)
{
	if (!TwEventMouseMotionGLUT(x, y))
	{
		int dx = x - mouseX;
		int dy = y - mouseY;

		if (leftDown && shifted) {
			scene.lightSpin += dx / 3.f;
			scene.lightTilt -= dy / 3.f;
		}

		else if (leftDown)
		{
			scene.gEditorCamera.horizontalMove((dx * global::timer::mDt));
			scene.gEditorCamera.verticalMove((dy * global::timer::mDt));
		}


		if (middleDown && shifted) {
			scene.lightDist = pow(scene.lightDist, 1.0f - dy / 200.0f);
		}

		else if (middleDown)
		{

		}


		if (rightDown && shifted) {
			/* nothing */
		}
		else if (rightDown) {
			scene.translatex += dx / 20.0f;
			scene.translatey -= dy / 20.0f;
			scene.gEditorCamera.setPanDir(glm::vec2(-dx * global::timer::mDt,
				                                    -dy * global::timer::mDt));
		}

		// Record this position 
		mouseX = x;
		mouseY = y;

		// Draw the scene, transformed by the new values.
		//glutPostRedisplay();
	}
}

void MouseWheel(int wheel, int direction, int x, int y)
{
	float cameraSpeed = 50.f;
	if (direction > 0)
	{
		scene.gEditorCamera.zoom(direction * global::timer::mDt);
	}
	else
	{
		scene.gEditorCamera.zoom(direction * global::timer::mDt);
	}
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////
// on exit callback function to clean up any resources allocated
void cleanUp()
{
	TwTerminate();
}

////////////////////////////////////////////////////////////////////////
// Do the OpenGL/GLut setup and then enter the interactive loop.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitContextVersion (4, 4);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);


	glutInitWindowSize(global::gWidth, global::gHeight);
    glutCreateWindow("WIP Graphics");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glewExperimental = TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Rendered by: %s\n", glGetString(GL_RENDERER));

    glutDisplayFunc(&ReDraw);
    glutReshapeFunc(&ReshapeWindow);
    glutKeyboardFunc(&KeyboardInput);
    glutMouseFunc(&MouseButton);
	glutMouseWheelFunc(&MouseWheel);
    glutMotionFunc(&MouseMotion);
	glutTimerFunc(1000.f/ 60.f, &TimerUpdate, 0);
	glutCloseFunc(cleanUp);

    InitializeScene(scene);

	global::timer::initializeTimer(0.0);
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize((int)global::gWidth, (int)global::gHeight);

	TwBar* atSceneControl = TwNewBar("Scene Control");
	TwBar* atLightControl = TwNewBar("Lighting Control");
	TwAddVarRO(atSceneControl, "FPS", TW_TYPE_FLOAT, &global::timer::mFPS, "");
	TwAddVarRW(atSceneControl, "Show G Buffer", TW_TYPE_BOOL8, &scene.showGBuffer, "");
	TwAddVarRW(atSceneControl, "Enable Gamma Correction", TW_TYPE_BOOL8, &scene.enableGammaCorrection, "");
	TwAddVarRW(atLightControl, "Ambient Light Color", TW_TYPE_COLOR3F, &scene.ambientLightParameters.ambientLightColor, "group=AmbientLight");
	TwAddVarRW(atLightControl, "Ambient Light Strength", TW_TYPE_FLOAT, &scene.ambientLightParameters.ambientLightStrength, "group=AmbientLight");
	for (unsigned int i = 0; i < scene.directionalLightParameters.size(); ++i)
	{
		std::stringstream group;
		group << "group=DirectionLight" << i;

		std::stringstream direction;
		direction << "Direction Light " << i << " Dir";

		std::stringstream diffuse;
		diffuse << "Direction Light " << i << " Diffuse Color";

		std::stringstream specular;
		specular << "Direction Light " << i << " Specular Color";

		TwAddVarRW(atLightControl, direction.str().c_str(), TW_TYPE_DIR3F, &scene.directionalLightParameters[i].directionLightDir, group.str().c_str());
		TwAddVarRW(atLightControl, diffuse.str().c_str(), TW_TYPE_COLOR3F, &scene.directionalLightParameters[i].directionLightDiffuse, group.str().c_str());
		TwAddVarRW(atLightControl, specular.str().c_str(), TW_TYPE_COLOR3F, &scene.directionalLightParameters[i].directionLightSpecular, group.str().c_str());
	}

	for (unsigned int i = 0; i < scene.pointLightParameters.size(); ++i)
	{
		std::stringstream ss;
		ss << "group=PointLight" << i;

		std::stringstream position;
		position << "Point Light " << i << " Position";

		std::stringstream diffuse;
		diffuse << "Point Light " << i << " Diffuse Color";

		std::stringstream specular;
		specular << "Point Light " << i << " Specular Color";

		std::stringstream distance;
		distance << "Point Light " << i << " Distance";

		std::stringstream constant;
		constant << "Point Light " << i << " Constant";

		std::stringstream linear;
		linear << "Point Light " << i << " Linear";

		std::stringstream quadratic;
		quadratic << "Point Light " << i << " Quadratic";

		TwAddVarRW(atLightControl, position.str().c_str(), TW_TYPE_DIR3F, &scene.pointLightParameters[i].pointLightPosition, ss.str().c_str());
		TwAddVarRW(atLightControl, diffuse.str().c_str(), TW_TYPE_COLOR3F, &scene.pointLightParameters[i].pointLightDiffuse, ss.str().c_str());
		TwAddVarRW(atLightControl, specular.str().c_str(), TW_TYPE_COLOR3F, &scene.pointLightParameters[i].pointLightSpecular, ss.str().c_str());

		TwAddVarRW(atLightControl, distance.str().c_str(), TW_TYPE_FLOAT, &scene.pointLightParameters[i].pointLightAttenuationDistance, ss.str().c_str());
		TwAddVarRW(atLightControl, constant.str().c_str(), TW_TYPE_FLOAT, &scene.pointLightParameters[i].pointLightAttenuationConstanst, ss.str().c_str());
		TwAddVarRW(atLightControl, linear.str().c_str(), TW_TYPE_FLOAT, &scene.pointLightParameters[i].pointLightAttenuationLinear, ss.str().c_str());
		TwAddVarRW(atLightControl, quadratic.str().c_str(), TW_TYPE_FLOAT, &scene.pointLightParameters[i].pointLightAttenuationQuadratic, ss.str().c_str());
	}

    // This function enters an event loop.
    glutMainLoop();
}
