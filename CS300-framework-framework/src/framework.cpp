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
#else
    // Includes for Linux
    #include <algorithm>
    #define sprintf_s sprintf
    #include <fstream>
#endif

#include <glload/gl_3_3.h>
#include <glload/gll.hpp>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "AntTweakBar.h"

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
	global::timer::updateTimer(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
    DrawScene(scene);
	TwDraw();
    glutSwapBuffers();
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
void KeyboardInput(unsigned char key, int, int)
{
    switch(key) {
	case 'd': scene.diffFlag =    1 - scene.diffFlag;  break;
	case 's': scene.specFlag =    2 - scene.specFlag;  break;
	case 't': scene.texFlag =     4 - scene.texFlag;  break;
	case 'w': scene.shadowFlag =  8 - scene.shadowFlag;  break;
	case 'b': scene.bumpFlag =   16 - scene.bumpFlag;  break;
	case 'r': scene.reflFlag =   32 - scene.reflFlag;  break;

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
        scene.mode = key-'0';
		glutPostRedisplay();
        break;

    case 27:                    // Escape key
        exit(0);
    }
}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse button changes state.
void MouseButton(int button, int state, int x, int y)
{
    shifted = glutGetModifiers() && GLUT_ACTIVE_SHIFT;

    if (button == GLUT_LEFT_BUTTON)
        leftDown = (state == GLUT_DOWN);

    else if (button == GLUT_MIDDLE_BUTTON)
        middleDown = (state == GLUT_DOWN);

    else if (button == GLUT_RIGHT_BUTTON)
        rightDown = (state == GLUT_DOWN);

    else if (button%8 == 3 && shifted)
        scene.lightDist = pow(scene.lightDist, 1.0f/1.02f);

    else if (button%8 == 3)
        scene.zoom = pow(scene.zoom, 1.0f/1.02f);

    else if (button%8 == 4 && shifted)
        scene.lightDist = pow(scene.lightDist, 1.02f);

    else if (button%8 == 4)
        scene.zoom = pow(scene.zoom, 1.02f);

    mouseX = x;
    mouseY = y;

	glutPostRedisplay();

}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse moves (while a button is down)
void MouseMotion(int x, int y)
{
    int dx = x-mouseX;
    int dy = y-mouseY;

    if (leftDown && shifted) {
        scene.lightSpin += dx/3.;
        scene.lightTilt -= dy/3.; }

    else if (leftDown) {
        scene.eyeSpin += dx/2.;
        scene.eyeTilt += dy/2.; }


    if (middleDown && shifted) {
        scene.lightDist = pow(scene.lightDist, 1.0f-dy/200.0f);  }

    else if (middleDown) {
        scene.zoom += dy/10.0f; }


    if (rightDown && shifted) {
        /* nothing */ }
    else if (rightDown) {
        scene.translatex += dx/20.0f;
        scene.translatey -= dy/20.0f; }

    // Record this position 
    mouseX = x;
    mouseY = y;

    // Draw the scene, transformed by the new values.
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


	glutInitWindowSize(global::gWidth,global::gHeight);
    glutCreateWindow("WIP Graphics");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glload::LoadFunctions();

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Rendered by: %s\n", glGetString(GL_RENDERER));

    glutDisplayFunc(&ReDraw);
    glutReshapeFunc(&ReshapeWindow);
    glutKeyboardFunc(&KeyboardInput);
    glutMouseFunc(&MouseButton);
    glutMotionFunc(&MouseMotion);
	glutCloseFunc(cleanUp);

    InitializeScene(scene);

	global::timer::initializeTimer(0.0);
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(global::gWidth, global::gHeight);

	TwBar* atTimerDIsplay = TwNewBar("Timer DIsplay");
	TwAddVarRO(atTimerDIsplay, "Delta Time", TW_TYPE_DOUBLE, &global::timer::mDt, "");
	TwAddVarRO(atTimerDIsplay, "FPS", TW_TYPE_DOUBLE, &global::timer::mFPS, "");
    // This function enters an event loop.
    glutMainLoop();
}
