///////////////////////////////////////////////////////////////////////
// A slight encapsulation of a Frame Buffer Object (i'e' Render
// Target) and its associated texture.  When the FBO is "Bound", the
// output of the graphics pipeline is captured into the texture.  When
// it is "Unbound", the texture is available for use as any normal
// texture.
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "shader.h"
#include "fbo.h"
#include "scene.h"
#include "GL\glew.h"
#include <GL/freeglut.h>

void FBO::CreateFBO(const int w, const int h)
{
	width = w;
	height = h;

    glGenFramebuffersEXT(1, &fbo);  
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        
    // Create a render buffer, and attach it to FBO's depth attachment
    unsigned int depthBuffer;
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
							 width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depthBuffer);

    // Create texture and attach FBO's color 0 attachment
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, texture, 0);

    // Check for completeness/correctness
    int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
        printf("FBO Error: %d\n", status);

    // Unbind the fbo.  
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

unsigned int FBO::getFBO()
{
	return fbo;
}

unsigned int FBO::getFBOTexture()
{
	return texture;
}

void FBO::getWidthAndHeight(int& FBOWidth, int& FBOHeight)
{
	FBOWidth = width;
	FBOHeight = height;
}

void FBO::setWidthAndHeight(const int &FBOWidth, const int &FBOHeight)
{
	if (width != FBOWidth || height != FBOHeight)
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);

		CreateFBO(FBOWidth, FBOHeight);
	}
}

void FBO::Bind() { glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); }
void FBO::Unbind() { glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); }
