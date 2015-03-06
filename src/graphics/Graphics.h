#ifndef GRAPHICS_MAIN
#define GRAPHICS_MAIN

#ifdef __cplusplus
extern "C" {
#endif

#include "VectorUtils3.h"
#include "LoadObject.h"

/**
 * Draws the object with the specified transform.
 *
 * @param transform The transform to be applied to the model.
 * @param model The model to be drawn.
 */
void drawObject(mat4 transform, Model* model);

vec3 moveOnKeyInput(GLfloat x, GLfloat y, GLfloat z);

vec3 moveOnKeyInputRelativeCamera(vec3 in);

void handleMouse(int x, int y);

void initGraphics(void);

int graphicsMain(int, char*[]);

	void onTimer(int);

#ifdef __cplusplus
}
#endif

#endif
