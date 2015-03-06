#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Graphics.h"
#include "GLUtilities.h"
#include "LoadObject.h"

Model* bunny;

void initGraphics(int argc, char** argv) {
	int* pargc = &argc;
	glutInit(pargc, argv);
	glutInitContextVersion(3, 2);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(400, 300);
	glutCreateWindow("Leap Music!");

  // Extension wrangler initialising
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
		exit(11);
  }

	if (initResources()){
		glutDisplayFunc(onDisplay);
		glutMainLoop();
	}
	else {
		printf("Failed to initialize resources, shutting down graphics.");
	}

	// Upon exiting the main loop;
	freeResources();
}


int initResources(void) {
	lookMatrix = lookAtv(cameraPosition, cameraTarget, cameraNormal);
	transHand = T(0, 0, 0);
	GLfloat viewFrustum[] = {2.0f*near/(right-left), 0.0f,
								 (right+left)/(right-left), 0.0f,
								 0.0f, 2.0f*near/(top-bottom),
								 (top+bottom)/(top-bottom), 0.0f,
								 0.0f, 0.0f, -(far + near)/(far - near),
								 -2*far*near/(far - near),
									0.0f, 0.0f, -1.0f, 0.0f };
	handProgram = loadShaders("./src/graphics/shaders/hand.vert",
	                          "./src/graphics/shaders/hand.frag");
	bunny = LoadModelPlus("./src/graphics/models/bunnyplus.obj");
	printf("lol\n");
	glUniformMatrix4fv(glGetUniformLocation(handProgram, "viewFrustum"),
										 1, GL_TRUE, viewFrustum);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutTimerFunc(16, &onTimer, 0);

	return 1;
}

void onDisplay(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(handProgram);
	drawObject(transHand, bunny, handProgram);
}

void freeResources(void) {
	;
}

void drawObject(mat4 transform, Model* model, GLuint p) {
	glUniformMatrix4fv(glGetUniformLocation(p, "transform"),
	                   1, GL_TRUE, transform.m);

	DrawModel(model, p, "vertPosition", "vertNormal", "vertTexureCoordinate");
}

void onTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(20, &onTimer, value);
}
