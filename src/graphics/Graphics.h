#ifndef LEAPMUSIC_GRAPHICS
#define LEAPMUSIC_GRAPHICS

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/glew.h>

#include "VectorUtils3.h"
#include "LoadObject.h"


	void initGraphics(int argc, char** argv);

	static int initResources(void);
	static void freeResources(void);
	static void onDisplay(void);
	static void onTimer(int);
	static void drawObject(mat4, Model*, GLuint);

	const GLfloat near = 1.0;
	const GLfloat far = 300.0;
	const GLfloat right = 0.5;
	const	GLfloat left = -0.5;
	const GLfloat top = 0.5;
	const GLfloat bottom = -0.5;
	const vec3 cameraPosition = {10.5f, 13.6f, 23.0f};
	const vec3 cameraTarget = {-10.0f, -10.0f, -10.0f};
	const vec3 cameraNormal = {0.0f, 1.0f, 0.0f};
	mat4 lookMatrix;
	mat4 transHand;
	Model* bunny;
	GLuint handProgram;
#ifdef __cplusplus
}
#endif

#endif
