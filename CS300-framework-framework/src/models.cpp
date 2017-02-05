////////////////////////////////////////////////////////////////////////
// A small library of object shapes (ground plane, sphere, and the
// famous Utah teapot), each created as a batch of Vertex Buffer
// Objects collected under a Vertex Array Object umbrella.  This is
// the latest and most efficient way to get geometry into the OpenGL
// graphics pipeline.
//
// Each vertex is specified as four attributes which are made
// available in a vertes shader in the following attribute slots.
//
// position,		vec4,	attribute #0
// normal,			vec3,	attribute #1
// texture coord,	vec3,	attribute #2
// tangent,			vec3,	attribute #3
//
// An instance of any of these shapes is create with a single call:
//    unsigned int obj = CreateSphere(divisions, &quadCount);
// and drawn by:
//	  glBindVertexArray(obj);
//	  glDrawElements(GL_QUADS, quadCount, GL_UNSIGNED_INT, 0);
//	  glBindVertexArray(0);
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "models.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "GL\glew.h"

#include "math.h"

#include "document.h"
#include "reader.h"
#include <istreamwrapper.h>
const float PI = 3.14159f;
const float rad = PI/180.0f;

////////////////////////////////////////////////////////////////////////////////
// Create a Vertex Array Object from (1) a collection of arrays
// containing vertex data and (2) a set of indices inticating quads.
// The arrays must all be the same length and contain respectively,
// the vertex position, normal, texture coordinate, and tangent
// vector.  This is the latest and most efficient way to get geometry
// into the OpenGL graphics pipeline.
unsigned int VaoFromArrays(int nv, int nq, float* Pnt, float* Nrm,
						   float* Tex, float* Tan, unsigned int* Ind)
{
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint Pbuff;
	glGenBuffers(1, &Pbuff);
	glBindBuffer(GL_ARRAY_BUFFER, Pbuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*nv, Pnt, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint Nbuff;
	glGenBuffers(1, &Nbuff);
	glBindBuffer(GL_ARRAY_BUFFER, Nbuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*nv, Nrm, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint Tbuff;
	glGenBuffers(1, &Tbuff);
	glBindBuffer(GL_ARRAY_BUFFER, Tbuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*nv, Tex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint Dbuff;
	glGenBuffers(1, &Dbuff);
	glBindBuffer(GL_ARRAY_BUFFER, Dbuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*nv, Tan, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint Ibuff;
	glGenBuffers(1, &Ibuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*4*nq, Ind, GL_STATIC_DRAW);

	glBindVertexArray(0);

	delete[] Pnt;
	delete[] Nrm;
	delete[] Tex;
	delete[] Tan;
	delete[] Ind;
	
	return vao;
}

////////////////////////////////////////////////////////////////////////////////
// Data for the Utah teapot.  It consists of a list of 306 control
// points, and 32 Bezier patches, each defined by 16 control points
// (specified as 1-based indices into the control point array).  This
// evaluates the Bernstein basis functions directly to compute the
// vertices.  This is not the most efficient, but it is the
// easiest. (Which is fine for an operation done once at startup
// time.)
unsigned int I[][16] = {
	  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 
	  4, 17, 18, 19,  8, 20, 21, 22, 12, 23, 24, 25, 16, 26, 27, 28, 
	 19, 29, 30, 31, 22, 32, 33, 34, 25, 35, 36, 37, 28, 38, 39, 40, 
	 31, 41, 42,  1, 34, 43, 44,  5, 37, 45, 46,  9, 40, 47, 48, 13, 
	 13, 14, 15, 16, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 
	 16, 26, 27, 28, 52, 61, 62, 63, 56, 64, 65, 66, 60, 67, 68, 69, 
	 28, 38, 39, 40, 63, 70, 71, 72, 66, 73, 74, 75, 69, 76, 77, 78, 
	 40, 47, 48, 13, 72, 79, 80, 49, 75, 81, 82, 53, 78, 83, 84, 57, 
	 57, 58, 59, 60, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 
	 60, 67, 68, 69, 88, 97, 98, 99, 92,100,101,102, 96,103,104,105,
	 69, 76, 77, 78, 99,106,107,108,102,109,110,111,105,112,113,114,
	 78, 83, 84, 57,108,115,116, 85,111,117,118, 89,114,119,120, 93,
	121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,
	124,137,138,121,128,139,140,125,132,141,142,129,136,143,144,133,
	133,134,135,136,145,146,147,148,149,150,151,152, 69,153,154,155,
	136,143,144,133,148,156,157,145,152,158,159,149,155,160,161, 69,
	162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,
	165,178,179,162,169,180,181,166,173,182,183,170,177,184,185,174,
	174,175,176,177,186,187,188,189,190,191,192,193,194,195,196,197,
	177,184,185,174,189,198,199,186,193,200,201,190,197,202,203,194,
	204,204,204,204,207,208,209,210,211,211,211,211,212,213,214,215,
	204,204,204,204,210,217,218,219,211,211,211,211,215,220,221,222,
	204,204,204,204,219,224,225,226,211,211,211,211,222,227,228,229,
	204,204,204,204,226,230,231,207,211,211,211,211,229,232,233,212,
	212,213,214,215,234,235,236,237,238,239,240,241,242,243,244,245,
	215,220,221,222,237,246,247,248,241,249,250,251,245,252,253,254,
	222,227,228,229,248,255,256,257,251,258,259,260,254,261,262,263,
	229,232,233,212,257,264,265,234,260,266,267,238,263,268,269,242,
	270,270,270,270,279,280,281,282,275,276,277,278,271,272,273,274,
	270,270,270,270,282,289,290,291,278,286,287,288,274,283,284,285,
	270,270,270,270,291,298,299,300,288,295,296,297,285,292,293,294,
	270,270,270,270,300,305,306,279,297,303,304,275,294,301,302,271};

float P[][3] = {
	1.4,0.0,2.4,
	1.4,-0.784,2.4,
	0.784,-1.4,2.4,
	0.0,-1.4,2.4,
	1.3375,0.0,2.53125,
	1.3375,-0.749,2.53125,
	0.749,-1.3375,2.53125,
	0.0,-1.3375,2.53125,
	1.4375,0.0,2.53125,
	1.4375,-0.805,2.53125,
	0.805,-1.4375,2.53125,
	0.0,-1.4375,2.53125,
	1.5,0.0,2.4,
	1.5,-0.84,2.4,
	0.84,-1.5,2.4,
	0.0,-1.5,2.4,
	-0.784,-1.4,2.4,
	-1.4,-0.784,2.4,
	-1.4,0.0,2.4,
	-0.749,-1.3375,2.53125,
	-1.3375,-0.749,2.53125,
	-1.3375,0.0,2.53125,
	-0.805,-1.4375,2.53125,
	-1.4375,-0.805,2.53125,
	-1.4375,0.0,2.53125,
	-0.84,-1.5,2.4,
	-1.5,-0.84,2.4,
	-1.5,0.0,2.4,
	-1.4,0.784,2.4,
	-0.784,1.4,2.4,
	0.0,1.4,2.4,
	-1.3375,0.749,2.53125,
	-0.749,1.3375,2.53125,
	0.0,1.3375,2.53125,
	-1.4375,0.805,2.53125,
	-0.805,1.4375,2.53125,
	0.0,1.4375,2.53125,
	-1.5,0.84,2.4,
	-0.84,1.5,2.4,
	0.0,1.5,2.4,
	0.784,1.4,2.4,
	1.4,0.784,2.4,
	0.749,1.3375,2.53125,
	1.3375,0.749,2.53125,
	0.805,1.4375,2.53125,
	1.4375,0.805,2.53125,
	0.84,1.5,2.4,
	1.5,0.84,2.4,
	1.75,0.0,1.875,
	1.75,-0.98,1.875,
	0.98,-1.75,1.875,
	0.0,-1.75,1.875,
	2.0,0.0,1.35,
	2.0,-1.12,1.35,
	1.12,-2.0,1.35,
	0.0,-2.0,1.35,
	2.0,0.0,0.9,
	2.0,-1.12,0.9,
	1.12,-2.0,0.9,
	0.0,-2.0,0.9,
	-0.98,-1.75,1.875,
	-1.75,-0.98,1.875,
	-1.75,0.0,1.875,
	-1.12,-2.0,1.35,
	-2.0,-1.12,1.35,
	-2.0,0.0,1.35,
	-1.12,-2.0,0.9,
	-2.0,-1.12,0.9,
	-2.0,0.0,0.9,
	-1.75,0.98,1.875,
	-0.98,1.75,1.875,
	0.0,1.75,1.875,
	-2.0,1.12,1.35,
	-1.12,2.0,1.35,
	0.0,2.0,1.35,
	-2.0,1.12,0.9,
	-1.12,2.0,0.9,
	0.0,2.0,0.9,
	0.98,1.75,1.875,
	1.75,0.98,1.875,
	1.12,2.0,1.35,
	2.0,1.12,1.35,
	1.12,2.0,0.9,
	2.0,1.12,0.9,
	2.0,0.0,0.45,
	2.0,-1.12,0.45,
	1.12,-2.0,0.45,
	0.0,-2.0,0.45,
	1.5,0.0,0.225,
	1.5,-0.84,0.225,
	0.84,-1.5,0.225,
	0.0,-1.5,0.225,
	1.5,0.0,0.15,
	1.5,-0.84,0.15,
	0.84,-1.5,0.15,
	0.0,-1.5,0.15,
	-1.12,-2.0,0.45,
	-2.0,-1.12,0.45,
	-2.0,0.0,0.45,
	-0.84,-1.5,0.225,
	-1.5,-0.84,0.225,
	-1.5,0.0,0.225,
	-0.84,-1.5,0.15,
	-1.5,-0.84,0.15,
	-1.5,0.0,0.15,
	-2.0,1.12,0.45,
	-1.12,2.0,0.45,
	0.0,2.0,0.45,
	-1.5,0.84,0.225,
	-0.84,1.5,0.225,
	0.0,1.5,0.225,
	-1.5,0.84,0.15,
	-0.84,1.5,0.15,
	0.0,1.5,0.15,
	1.12,2.0,0.45,
	2.0,1.12,0.45,
	0.84,1.5,0.225,
	1.5,0.84,0.225,
	0.84,1.5,0.15,
	1.5,0.84,0.15,
	-1.6,0.0,2.025,
	-1.6,-0.3,2.025,
	-1.5,-0.3,2.25,
	-1.5,0.0,2.25,
	-2.3,0.0,2.025,
	-2.3,-0.3,2.025,
	-2.5,-0.3,2.25,
	-2.5,0.0,2.25,
	-2.7,0.0,2.025,
	-2.7,-0.3,2.025,
	-3.0,-0.3,2.25,
	-3.0,0.0,2.25,
	-2.7,0.0,1.8,
	-2.7,-0.3,1.8,
	-3.0,-0.3,1.8,
	-3.0,0.0,1.8,
	-1.5,0.3,2.25,
	-1.6,0.3,2.025,
	-2.5,0.3,2.25,
	-2.3,0.3,2.025,
	-3.0,0.3,2.25,
	-2.7,0.3,2.025,
	-3.0,0.3,1.8,
	-2.7,0.3,1.8,
	-2.7,0.0,1.575,
	-2.7,-0.3,1.575,
	-3.0,-0.3,1.35,
	-3.0,0.0,1.35,
	-2.5,0.0,1.125,
	-2.5,-0.3,1.125,
	-2.65,-0.3,0.9375,
	-2.65,0.0,0.9375,
	-2.0,-0.3,0.9,
	-1.9,-0.3,0.6,
	-1.9,0.0,0.6,
	-3.0,0.3,1.35,
	-2.7,0.3,1.575,
	-2.65,0.3,0.9375,
	-2.5,0.3,1.125,
	-1.9,0.3,0.6,
	-2.0,0.3,0.9,
	1.7,0.0,1.425,
	1.7,-0.66,1.425,
	1.7,-0.66,0.6,
	1.7,0.0,0.6,
	2.6,0.0,1.425,
	2.6,-0.66,1.425,
	3.1,-0.66,0.825,
	3.1,0.0,0.825,
	2.3,0.0,2.1,
	2.3,-0.25,2.1,
	2.4,-0.25,2.025,
	2.4,0.0,2.025,
	2.7,0.0,2.4,
	2.7,-0.25,2.4,
	3.3,-0.25,2.4,
	3.3,0.0,2.4,
	1.7,0.66,0.6,
	1.7,0.66,1.425,
	3.1,0.66,0.825,
	2.6,0.66,1.425,
	2.4,0.25,2.025,
	2.3,0.25,2.1,
	3.3,0.25,2.4,
	2.7,0.25,2.4,
	2.8,0.0,2.475,
	2.8,-0.25,2.475,
	3.525,-0.25,2.49375,
	3.525,0.0,2.49375,
	2.9,0.0,2.475,
	2.9,-0.15,2.475,
	3.45,-0.15,2.5125,
	3.45,0.0,2.5125,
	2.8,0.0,2.4,
	2.8,-0.15,2.4,
	3.2,-0.15,2.4,
	3.2,0.0,2.4,
	3.525,0.25,2.49375,
	2.8,0.25,2.475,
	3.45,0.15,2.5125,
	2.9,0.15,2.475,
	3.2,0.15,2.4,
	2.8,0.15,2.4,
	0.0,0.0,3.15,
	0.0,-0.002,3.15,
	0.002,0.0,3.15,
	0.8,0.0,3.15,
	0.8,-0.45,3.15,
	0.45,-0.8,3.15,
	0.0,-0.8,3.15,
	0.0,0.0,2.85,
	0.2,0.0,2.7,
	0.2,-0.112,2.7,
	0.112,-0.2,2.7,
	0.0,-0.2,2.7,
	-0.002,0.0,3.15,
	-0.45,-0.8,3.15,
	-0.8,-0.45,3.15,
	-0.8,0.0,3.15,
	-0.112,-0.2,2.7,
	-0.2,-0.112,2.7,
	-0.2,0.0,2.7,
	0.0,0.002,3.15,
	-0.8,0.45,3.15,
	-0.45,0.8,3.15,
	0.0,0.8,3.15,
	-0.2,0.112,2.7,
	-0.112,0.2,2.7,
	0.0,0.2,2.7,
	0.45,0.8,3.15,
	0.8,0.45,3.15,
	0.112,0.2,2.7,
	0.2,0.112,2.7,
	0.4,0.0,2.55,
	0.4,-0.224,2.55,
	0.224,-0.4,2.55,
	0.0,-0.4,2.55,
	1.3,0.0,2.55,
	1.3,-0.728,2.55,
	0.728,-1.3,2.55,
	0.0,-1.3,2.55,
	1.3,0.0,2.4,
	1.3,-0.728,2.4,
	0.728,-1.3,2.4,
	0.0,-1.3,2.4,
	-0.224,-0.4,2.55,
	-0.4,-0.224,2.55,
	-0.4,0.0,2.55,
	-0.728,-1.3,2.55,
	-1.3,-0.728,2.55,
	-1.3,0.0,2.55,
	-0.728,-1.3,2.4,
	-1.3,-0.728,2.4,
	-1.3,0.0,2.4,
	-0.4,0.224,2.55,
	-0.224,0.4,2.55,
	0.0,0.4,2.55,
	-1.3,0.728,2.55,
	-0.728,1.3,2.55,
	0.0,1.3,2.55,
	-1.3,0.728,2.4,
	-0.728,1.3,2.4,
	0.0,1.3,2.4,
	0.224,0.4,2.55,
	0.4,0.224,2.55,
	0.728,1.3,2.55,
	1.3,0.728,2.55,
	0.728,1.3,2.4,
	1.3,0.728,2.4,
	0.0,0.0,0.0,
	1.5,0.0,0.15,
	1.5,0.84,0.15,
	0.84,1.5,0.15,
	0.0,1.5,0.15,
	1.5,0.0,0.075,
	1.5,0.84,0.075,
	0.84,1.5,0.075,
	0.0,1.5,0.075,
	1.425,0.0,0.0,
	1.425,0.798,0.0,
	0.798,1.425,0.0,
	0.0,1.425,0.0,
	-0.84,1.5,0.15,
	-1.5,0.84,0.15,
	-1.5,0.0,0.15,
	-0.84,1.5,0.075,
	-1.5,0.84,0.075,
	-1.5,0.0,0.075,
	-0.798,1.425,0.0,
	-1.425,0.798,0.0,
	-1.425,0.0,0.0,
	-1.5,-0.84,0.15,
	-0.84,-1.5,0.15,
	0.0,-1.5,0.15,
	-1.5,-0.84,0.075,
	-0.84,-1.5,0.075,
	0.0,-1.5,0.075,
	-1.425,-0.798,0.0,
	-0.798,-1.425,0.0,
	0.0,-1.425,0.0,
	0.84,-1.5,0.15,
	1.5,-0.84,0.15,
	0.84,-1.5,0.075,
	1.5,-0.84,0.075,
	0.798,-1.425,0.0,
	1.425,-0.798,0.0};

////////////////////////////////////////////////////////////////////////////////
// Builds a Vertex Array Object for the Utah teapot.  Each of the 32
// patches is represented by an n by n grid of quads.
unsigned int CreateTeapot(const int n, unsigned int& count)
{
	int npatches = sizeof(I)/sizeof(I[0]); // Should be 32 patches for the teapot
	const int nv = npatches*(n+1)*(n+1);
	int nq = npatches*n*n;
	float* Pnt = new float[4*nv];  float* pp = Pnt;
	float* Nrm = new float[3*nv];  float* np = Nrm;
	float* Tex = new float[2*nv];  float* tp = Tex;
	float* Tan = new float[3*nv];  float* dp = Tan;
	unsigned int* Ind = new unsigned int[4*nq];  unsigned int* ip = Ind;

	for (int p=0;  p<npatches;  p++)	{ // For each patch
		for (int i=0;  i<=n; i++) {		  // Grid in u direction
			float u = float(i)/n;
			for (int j=0;  j<=n; j++) { // Grid if v direction
				float v = float(j)/n;

				// Four u weights
				float u0 = (1.0-u)*(1.0-u)*(1.0-u);
				float u1 = 3.0*(1.0-u)*(1.0-u)*u;
				float u2 = 3.0*(1.0-u)*u*u;
				float u3 = u*u*u;

				// Three du weights
				float du0 = (1.0-u)*(1.0-u);
				float du1 = 2.0*(1.0-u)*u;
				float du2 = u*u;

				// Four v weights
				float v0 = (1.0-v)*(1.0-v)*(1.0-v);
				float v1 = 3.0*(1.0-v)*(1.0-v)*v;
				float v2 = 3.0*(1.0-v)*v*v;
				float v3 = v*v*v;

				// Three dv weights
				float dv0 = (1.0-v)*(1.0-v);
				float dv1 = 2.0*(1.0-v)*v;
				float dv2 = v*v;
				
				// Grab pointers to the 16 control points for Bezier patch.
				float* p00 = P[I[p][ 0]-1];
				float* p01 = P[I[p][ 1]-1];
				float* p02 = P[I[p][ 2]-1];
				float* p03 = P[I[p][ 3]-1];
				float* p10 = P[I[p][ 4]-1];
				float* p11 = P[I[p][ 5]-1];
				float* p12 = P[I[p][ 6]-1];
				float* p13 = P[I[p][ 7]-1];
				float* p20 = P[I[p][ 8]-1];
				float* p21 = P[I[p][ 9]-1];
				float* p22 = P[I[p][10]-1];
				float* p23 = P[I[p][11]-1];
				float* p30 = P[I[p][12]-1];
				float* p31 = P[I[p][13]-1];
				float* p32 = P[I[p][14]-1];
				float* p33 = P[I[p][15]-1];

				float du[3];  float* pu=du;
				float dv[3];  float* pv=dv;

				for (int c=0;  c<3; c++) {
					// Evaluate the Bezier patch at (u,v)
					*pp++ =
						u0*v0*(*p00) + u0*v1*(*p01) + u0*v2*(*p02) + u0*v3*(*p03) + 
						u1*v0*(*p10) + u1*v1*(*p11) + u1*v2*(*p12) + u1*v3*(*p13) + 
						u2*v0*(*p20) + u2*v1*(*p21) + u2*v2*(*p22) + u2*v3*(*p23) + 
						u3*v0*(*p30) + u3*v1*(*p31) + u3*v2*(*p32) + u3*v3*(*p33); 
					
					// Evaluate the u-tangent of the Bezier patch at (u,v)
					*pu++ = *dp++ = 
						du0*v0*(*p10-*p00) + du0*v1*(*p11-*p01) + du0*v2*(*p12-*p02) + du0*v3*(*p13-*p03) + 
						du1*v0*(*p20-*p10) + du1*v1*(*p21-*p11) + du1*v2*(*p22-*p12) + du1*v3*(*p23-*p13) + 
						du2*v0*(*p30-*p20) + du2*v1*(*p31-*p21) + du2*v2*(*p32-*p22) + du2*v3*(*p33-*p23); 

					// Evaluate the v-tangent of the Bezier patch at (u,v)
					*pv++ = 
						u0*dv0*(*p01-*p00) + u0*dv1*(*p02-*p01) + u0*dv2*(*p03-*p02) + 
						u1*dv0*(*p11-*p10) + u1*dv1*(*p12-*p11) + u1*dv2*(*p13-*p12) +
						u2*dv0*(*p21-*p20) + u2*dv1*(*p22-*p21) + u2*dv2*(*p23-*p22) +
						u3*dv0*(*p31-*p30) + u3*dv1*(*p32-*p31) + u3*dv2*(*p33-*p32); 

					p00++; p01++; p02++; p03++; 
				    p10++; p11++; p12++; p13++; 
					p20++; p21++; p22++; p23++; 
					p30++; p31++; p32++; p33++; }

				*pp++ = 1.0;

				// Calculate the surface notmal as the cross product of the two tangents.
				*np++ = -(du[1]*dv[2]-du[2]*dv[1]);
				*np++ = -(du[2]*dv[0]-du[0]*dv[2]);
				*np++ = -(du[0]*dv[1]-du[1]*dv[0]);

				// Create a quad for all but the first edge vertices
				if (i>0 && j>0) {
					*ip++ = p*(n+1)*(n+1) + (i-1)*(n+1) + (j-1);
					*ip++ = p*(n+1)*(n+1) + (i-1)*(n+1) + (j);
					*ip++ = p*(n+1)*(n+1) + (i  )*(n+1) + (j); 
					*ip++ = p*(n+1)*(n+1) + (i  )*(n+1) + (j-1); } } } }

	GLuint vao = VaoFromArrays(nv, nq, Pnt, Nrm, Tex, Tan, Ind);
	count = 4*nq;
	return vao;
}



////////////////////////////////////////////////////////////////////////
// Generates a sphere with normals, texture coords, and tangent vectors.
unsigned int CreateSphere(const int n, unsigned int& count)
{
	const int nv = (2*n+1)*(n+1);
	int nq = 2*n*n;
	float* Pnt = new float[4*nv];  float* pp = Pnt;
	float* Nrm = new float[3*nv];  float* np = Nrm;
	float* Tex = new float[2*nv];  float* tp = Tex;
	float* Tan = new float[3*nv];  float* dp = Tan;
	unsigned int* Ind = new unsigned int[4*nq];  unsigned int* ip = Ind;
    float d = 2.0f*PI/float(n*2);
    for (int i=0;  i<=n*2;  i++) {
        float s = i*2.0f*PI/float(n*2);
        for (int j=0;  j<=n;  j++) {
            float t = j*PI/float(n);
            float x = cos(s)*sin(t);
			float y = sin(s)*sin(t);
			float z = cos(t);
            *pp++ = x;			*pp++ = y;		*pp++ = z;	*pp++ = 1.0;
            *np++ = x;			*np++ = y;		*np++ = z;
			*tp++ = s/(2*PI);	*tp++ = t/PI;
            *dp++ = -sin(s);	*dp++ = cos(s);	*dp++ = 0.0;
			if (i>0 && j>0) {
				*ip++ = (i-1)*(n+1) + (j-1);
				*ip++ = (i-1)*(n+1) + (j);
				*ip++ = (i  )*(n+1) + (j); 
				*ip++ = (i  )*(n+1) + (j-1); } } }
	
	GLuint vao = VaoFromArrays(nv, nq, Pnt, Nrm, Tex, Tan, Ind);
	count = 4*nq;
	return vao;
}

////////////////////////////////////////////////////////////////////////
// Generates a plane with normals, texture coords, and tangent vectors
// from an n by n grid of small quads.  A single quad might have been
// sufficient, but that works poorly with the the reflection map.
unsigned int CreateGround(const float r, const int n, unsigned int& count)
{
	const int nv = (n+1)*(n+1);
	int nq = n*n;
	float* Pnt = new float[4*nv];  float* pp = Pnt;
	float* Nrm = new float[3*nv];  float* np = Nrm;
	float* Tex = new float[2*nv];  float* tp = Tex;
	float* Tan = new float[3*nv];  float* dp = Tan;
	unsigned int* Ind = new unsigned int[4*nq];  unsigned int* ip = Ind;

    for (int i=0;  i<=n;  i++) {
        float s = i/float(n);
        for (int j=0;  j<=n;  j++) {
            float t = j/float(n);
            *pp++ = s*2.0*r-r;	*pp++ = t*2.0*r-r;	*pp++ = -3.0;	*pp++ = 1.0;
            *np++ = 0.0;		*np++ = 0.0;		*np++ = 1.0;
			*tp++ = s;			*tp++ = t;
            *dp++ = 1.0;		*dp++ = 0.0;		*dp++ = 0.0;
			if (i>0 && j>0) {
				*ip++ = (i-1)*(n+1) + (j-1);
				*ip++ = (i-1)*(n+1) + (j);
				*ip++ = (i  )*(n+1) + (j); 
				*ip++ = (i  )*(n+1) + (j-1); } } }
	
	GLuint vao = VaoFromArrays(nv, nq, Pnt, Nrm, Tex, Tan, Ind);
	count = 4*nq;
	return vao;
}

bool loadModelFromFile(const char *path, meshData &mesh)
{
	std::ifstream ifs(path, std::ifstream::in | std::ifstream::binary);
	if (!ifs.is_open())
	{
		std::cout << "Error is opening file " << path << std::endl;
		return false;
	}
		
	rapidjson::IStreamWrapper isw(ifs);

	rapidjson::Document d;
	auto& jsonDoc = d.ParseStream(isw);
	std::cout << "Model file: " << jsonDoc["ModelFile"].GetString() << std::endl;
	mesh.meshName = jsonDoc["MeshName"].GetString();
	std::cout << "Mesh name: " << mesh.meshName << std::endl;
	const rapidjson::Value& meshVertices = jsonDoc["Vertices"];
	for (int i = 0; i < meshVertices.Size(); ++i)
	{
		const rapidjson::Value& currentVert = meshVertices[i];
		glm::vec3 vert(currentVert[0].GetDouble(),
			currentVert[1].GetDouble(),
			currentVert[2].GetDouble());
		mesh.verts.push_back(vert);
	}
	
	auto meshIndices = jsonDoc["Indices"].GetArray();
	for (unsigned int i = 0; i < meshIndices.Size(); ++i)
	{
		mesh.faces.push_back(meshIndices[i].GetUint());
	}

	auto meshNormals = jsonDoc["Normals"].GetArray();
	for (unsigned int i = 0; i < meshNormals.Size(); ++i)
	{
		glm::vec3 normal(meshNormals[i][0].GetDouble(),
			meshNormals[i][1].GetDouble(),
			meshNormals[i][2].GetDouble());
		mesh.normals.push_back(normal);
	}

	auto meshTexCoord = jsonDoc["TexCoords"].GetArray();
	for (unsigned int i = 0; i < meshTexCoord.Size(); ++i)
	{
		glm::vec3 uv(meshTexCoord[i][0].GetDouble(),
			meshTexCoord[i][1].GetDouble(),
			meshTexCoord[i][2].GetDouble());
		mesh.uvs.push_back(uv);
	}

	auto meshTangents = jsonDoc["Tangents"].GetArray();
	for (unsigned int i = 0; i < meshTangents.Size(); ++i)
	{
		glm::vec3 tan(meshTangents[i][0].GetDouble(),
			meshTangents[i][1].GetDouble(),
			meshTangents[i][2].GetDouble());
		mesh.tans.push_back(tan);
	}

	auto meshBiTans = jsonDoc["BiTangents"].GetArray();
	for (unsigned int i = 0; i < meshBiTans.Size(); ++i)
	{
		glm::vec3 biTan(meshBiTans[i][0].GetDouble(),
			meshBiTans[i][1].GetDouble(),
			meshBiTans[i][2].GetDouble());
		mesh.biTans.push_back(biTan);
	}

	return true;
}

unsigned int createVAO(meshData& mesh)
{
	unsigned int vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vertices;
	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.verts.size(), &mesh.verts.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int normals;
	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), &mesh.normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	unsigned int uv;
	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.uvs.size(), &mesh.uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int tan;
	glGenBuffers(1, &tan);
	glBindBuffer(GL_ARRAY_BUFFER, tan);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.tans.size(), &mesh.tans.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int bitan;
	glGenBuffers(1, &bitan);
	glBindBuffer(GL_ARRAY_BUFFER, bitan);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.biTans.size(), &mesh.biTans.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indicies;
	glGenBuffers(1, &indicies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicies);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.faces.size(), &mesh.faces.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	return vao;
}

unsigned int createQuad(unsigned int &faceCount)
{

	//std::vector<glm::vec3> vertices = {
	//	glm::vec3(0.5f,  0.5f, 0.0f),  // Top Right
	//	glm::vec3(0.5f, -0.5f, 0.0f),  // Bottom Right
	//	glm::vec3(-0.5f, -0.5f, 0.0f),  // Bottom Left
	//	glm::vec3(-0.5f,  0.5f, 0.0f)   // Top Left 
	//};
	//std::vector<unsigned int> faces = {  // Note that we start from 0!
	//	0, 1, 3,   // First Triangle
	//	1, 2, 3    // Second Triangle
	//};

	//std::vector<glm::vec3> uvs = {
	//	glm::vec3(1.f,1.f,0.f),
	//	glm::vec3(1.f, 0.f, 0.f),
	//	glm::vec3(0.f, 0.f, 0.f),
	//	glm::vec3(0.f, 1.f, 0.f)
	//};

	//meshData quadData;
	//quadData.faces = faces;
	//quadData.uvs = uvs;
	//quadData.verts = vertices;
	//quadData.biTans = std::vector<glm::vec3>();
	//quadData.tans = std::vector<glm::vec3>();
	//quadData.normals = std::vector<glm::vec3>();
	//faceCount = faces.size();
	//return createVAO(quadData);

	glm::vec3 vertices[] = {
		// Positions         
		glm::vec3(1.f,  1.f, 0.0f),   
		glm::vec3(1.f, -1.f, 0.0f),   
		glm::vec3(-1.f, -1.f, 0.0f),  
		glm::vec3(-1.f,  1.f, 0.0f)
	};

	glm::vec2 texCoords[] = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int uv;
	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO
	faceCount = 6;
	return VAO;

}
