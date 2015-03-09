#include <iostream>

#include <GL/glew.h>
#include "MicroGlut.h"

#include "Graphics.hpp"
#include "GLUtilities.h"
#include "LoadObject.h"
#include "LoadTGA.h"


// These initializations are here because C++ sucks.
// They should be in the header file in my opinion.
const vec3 Graphics::cameraPosition = vec3(0.0f, 20.0f, 40.0f);
const vec3 Graphics::cameraTarget = vec3(0.0f, 20.0f, 0.0f);
const vec3 Graphics::cameraNormal = vec3(0.0f, 1.0f, 0.0f);
const mat4 Graphics::lookMatrix =
	lookAtv(cameraPosition, cameraTarget, cameraNormal);
const GLfloat Graphics::viewFrustum[] =
	{2.0f*near/(right-left), 0.0f,
	 (right+left)/(right-left), 0.0f,
	 0.0f, 2.0f*near/(top-bottom),
	 (top+bottom)/(top-bottom), 0.0f,
	 0.0f, 0.0f, -(far + near)/(far - near),
	 -2*far*near/(far - near),
	 0.0f, 0.0f, -1.0f, 0.0f };

// The following will be reassigned in initResources.
GLuint Graphics::program = 0;
Model* Graphics::bunny = NULL;
GLuint Graphics::grass = 0;
GLuint Graphics::concrete = 0;
GLuint Graphics::red = 0;
mat4 Graphics::transHand = T(0, 0, 0);

// The following will immediately be overwritten by Consumer.
std::atomic<float> Graphics::handX = {0};
std::atomic<float> Graphics::handY = {0};
std::atomic<bool>  Graphics::playing = {false};
std::atomic<bool>  Graphics::recording = {false};


void Graphics::init(int argc, char** argv) {
	int* pargc = &argc;
	glutInit(pargc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(300, 200);
	glutCreateWindow("Leap Music!");
	printError("Graphics::init()");

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	printError("Graphics::init() in call to glewInit()");
	if (glew_status != GLEW_OK) {
		std::cout << glewGetErrorString(glew_status) << std::endl;
	}

	dumpInfo();

	if (initResources()) {
		/* We can display it if everything goes OK */
		glutDisplayFunc(Graphics::onDisplay);
		glutMainLoop();
		std::cout << "Exited main graphics loop." << std::endl;
	}

	// Upon exiting the main loop;
	freeResources();
}


int Graphics::initResources(void) {
	program = loadShaders((char*)"./src/graphics/shaders/main.vert",
												(char*)"./src/graphics/shaders/main.frag");
	bunny = LoadModelPlus((char*)"./src/graphics/models/bunnyplus.obj");
	glUniformMatrix4fv(glGetUniformLocation(program, "viewFrustum"),
	                   1, GL_TRUE, viewFrustum);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookMatrix"),
										 1, GL_TRUE, lookMatrix.m);

	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	LoadTGATextureSimple((char*)"./src/graphics/textures/grass.tga", &grass);
	LoadTGATextureSimple((char*)"./src/graphics/textures/concrete.tga", &concrete);
	LoadTGATextureSimple((char*)"./src/graphics/textures/red.tga", &red);

	glUseProgram(program);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glutTimerFunc(16, &Graphics::onTimer, 0);

	printError("Graphics::initResources()");
	return 1;
}

void Graphics::onDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	transHand = Mult(T(handX / 10, handY / 10, 0), Ry(M_PI_2));

	if (recording)
		glBindTexture(GL_TEXTURE_2D, red);
	else if(playing)
		glBindTexture(GL_TEXTURE_2D, grass);
	else
		glBindTexture(GL_TEXTURE_2D, concrete);

	drawObject(transHand, bunny, program);

	glutSwapBuffers();
	printError("Graphics::onDisplay()");
}

void Graphics::freeResources(void) {
	printError("Graphics::freeResources()");
}

void Graphics::drawObject(mat4 transform, Model* model, GLuint p) {
	glUniformMatrix4fv(glGetUniformLocation(p, "transform"),
	                   1, GL_TRUE, transform.m);
	DrawModel(model, p, (char*)"vertPosition",
						(char*)"vertNormal", (char*)"vertTexCoord");
	printError("Graphics::drawObject()");
}

void Graphics::onTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(16, &Graphics::onTimer, value);
	printError("Graphics::onTimer()");

}
