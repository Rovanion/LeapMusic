#include <iostream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Graphics.hpp"
#include "GLUtilities.h"
#include "LoadObject.h"


// These initializations are here because C++ sucks.
// They should be in the header file in my opinion.
const vec3 Graphics::cameraPosition = vec3(10.5f, 13.6f, 23.0f);
const vec3 Graphics::cameraTarget = vec3(-10.0f, -10.0f, -10.0f);
const vec3 Graphics::cameraNormal = vec3(0.0f, 1.0f, 0.0f);
const mat4 Graphics::lookMatrix =
	lookAtv(cameraPosition, cameraTarget, cameraNormal);
const mat4 Graphics::transHand = T(0, 0, 0);
const GLfloat Graphics::viewFrustum[] =
	{2.0f*near/(right-left), 0.0f,
	 (right+left)/(right-left), 0.0f,
	 0.0f, 2.0f*near/(top-bottom),
	 (top+bottom)/(top-bottom), 0.0f,
	 0.0f, 0.0f, -(far + near)/(far - near),
	 -2*far*near/(far - near),
	 0.0f, 0.0f, -1.0f, 0.0f };
Model* Graphics::bunny = new Model;

// The following will be reassigned in initResources.
GLuint Graphics::handProgram = 0;

void Graphics::init(int argc, char** argv) {
	int* pargc = &argc;
	glutInit(pargc, argv);
	glutInitContextVersion(3, 2);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(400, 300);
	glutCreateWindow("Leap Music!");

 	if (DEBUG)
		std::cout << "Before init" << std::endl;

  // Extension wrangler initialising
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(glew_status) << std::endl;
		exit(11);
  }
	if (DEBUG)
		std::cout << "After main loop" << std::endl;
	std::cout << bunny << std::endl;
	bunny = initResources();
	std::cout << "wabababa" << std::endl;
	std::cout << bunny << std::endl;

	glutDisplayFunc(onDisplay);
	glutMainLoop();


	// Upon exiting the main loop;
	freeResources();
}


Model* Graphics::initResources(void) {
	if (DEBUG)
		std::cout << "In init" << std::endl;
	handProgram = loadShaders("./src/graphics/shaders/hand.vert",
	                          "./src/graphics/shaders/hand.frag");
	glUniformMatrix4fv(glGetUniformLocation(handProgram, "viewFrustum"),
										 1, GL_TRUE, viewFrustum);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutTimerFunc(16, &Graphics::onTimer, 0);

	return LoadModelPlus("./src/graphics/models/bunnyplus.obj");
}

void Graphics::onDisplay(void) {
	std::cout << "lol" << std::endl;
	std::cout << bunny->numVertices << std::endl;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(handProgram);
	drawObject(transHand, bunny, handProgram);
}

void Graphics::freeResources(void) {
	;
}

void Graphics::drawObject(mat4 transform, Model* model, GLuint p) {
	glUniformMatrix4fv(glGetUniformLocation(p, "transform"),
	                   1, GL_TRUE, transform.m);

	drawModel(model, p, "vertPosition", "vertNormal", "vertTexureCoordinate");
}

void Graphics::onTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(20, &Graphics::onTimer, value);
}


void Graphics::drawModel(Model *m,
												 GLuint program,
												 char* vertexVariableName,
												 char* normalVariableName,
												 char* texCoordVariableName) {

	std::cout << "lol2" << m->vao << std::endl;

	if (m) {
		GLint loc;

		glBindVertexArray(m->vao);	// Select VAO

		glBindBuffer(GL_ARRAY_BUFFER, m->vb);

		loc = glGetAttribLocation(program, vertexVariableName);
		if (loc >= 0)
			{
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(loc);
			}
		else
			fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", vertexVariableName);

		if (normalVariableName)
			{
				loc = glGetAttribLocation(program, normalVariableName);
				if (loc >= 0)
					{
						glBindBuffer(GL_ARRAY_BUFFER, m->nb);
						glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
						glEnableVertexAttribArray(loc);
					}
				else
					fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", normalVariableName);
			}

		// VBO for texture coordinate data NEW for 5b
		if ((m->texCoordArray != NULL)&&(texCoordVariableName != NULL))
			{
				loc = glGetAttribLocation(program, texCoordVariableName);
				if (loc >= 0)
					{
						glBindBuffer(GL_ARRAY_BUFFER, m->tb);
						glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
						glEnableVertexAttribArray(loc);
					}
				else
					fprintf(stderr, "DrawModel warning: '%s' not found in shader!\n", texCoordVariableName);
			}

		glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
	}
	else
		std::cout << "DrawModel error: Nullpointer passed!\n";
}
