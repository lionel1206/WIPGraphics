///////////////////////////////////////////////////////////////////////
// A slight encapsulation of a Frame Buffer Object (i'e' Render
// Target) and its associated texture.  When the FBO is "Bound", the
// output of the graphics pipeline is captured into the texture.  When
// it is "Unbound", the texture is available for use as any normal
// texture.
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#pragma once

class FBO {
public:
	void CreateFBO(const int w, const int h);
	void Bind();
	void Unbind();
	void getWidthAndHeight(int& FBOWidth, int& FBOHeight);
	void setWidthAndHeight(const int &FBOWidth, const int &FBOHeight);
	unsigned int getFBO();
	unsigned int getFBOTexture();
private:
	unsigned int fbo;
	unsigned int texture;
	int width, height;  // Size of the texture.
	
};
