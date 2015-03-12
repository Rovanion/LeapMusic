#include <iostream>

#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Graphics.hpp"
#include "GLUtilities.h"
#include "LoadObject.h"
#include "LoadTGA.h"


// These initializations are here because C++ sucks.
// They should be in the header file in my opinion.
const vec3 Graphics::cameraPosition = vec3(0.0f, 20.0f, 10.0f);
const vec3 Graphics::cameraTarget = vec3(0.0f, 20.0f, 0.0f);
const vec3 Graphics::cameraNormal = vec3(0.0f, 1.0f, 0.0f);
const mat4 Graphics::lookMatrix =
	lookAtv(cameraPosition, cameraTarget, cameraNormal);
const GLfloat Graphics::viewFrustum[] =
	{0.07f/(right-left), 0.0f, 0.0f, 0.0f,
	 0.0f, 0.07f/(top-bottom), 0.0f, 0.0f,
	 0.0f, 0.0f, 1/(far - near), -near/(far - near),
	 0.0f, 0.0f, 0.0f, 1.0f };


// The following will be reassigned in initResources.
GLuint Graphics::program     = 0;
Model* Graphics::bunny       = NULL;
Model* Graphics::note        = NULL;
Model* Graphics::plane       = NULL;
GLuint Graphics::green       = 0;
GLuint Graphics::white       = 0;
GLuint Graphics::red         = 0;
GLuint Graphics::darkBlue    = 0;
GLuint Graphics::clef        = 0;
GLuint Graphics::signTexture = 0;
mat4   Graphics::transHand   = T(0, 0, 0);
mat4   Graphics::transSign   = Mult(T(0, 20, 0), Rx(-M_PI_2));
mat4   Graphics::transPlane  = Mult(Mult(T(0, 20, 1), Rx(-M_PI_2)), S(1.5, 1.5, 1.5));
std::atomic<bool> Graphics::signShowing = {false};
std::atomic<bool> Graphics::loadNewSign = {false};
std::atomic<bool> Graphics::menuOpen = {false};
std::string Graphics::signTexturePath = "";

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
	glutInitWindowSize(1024, 768);
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
	note = LoadModelPlus((char*)"./src/graphics/models/note.obj");
	plane = LoadModelPlus((char*)"./src/graphics/models/plane2.obj");
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "viewFrustum"),
	                   1, GL_TRUE, viewFrustum);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookMatrix"),
										 1, GL_TRUE, lookMatrix.m);

	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	LoadTGATextureSimple((char*)"./src/graphics/textures/green.tga", &green);
	LoadTGATextureSimple((char*)"./src/graphics/textures/white.tga", &white);
	LoadTGATextureSimple((char*)"./src/graphics/textures/red.tga", &red);
	LoadTGATextureSimple((char*)"./src/graphics/textures/darkblue.tga", &darkBlue);
	LoadTGATextureSimple((char*)"./src/graphics/textures/clef_short.tga", &clef);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glutTimerFunc(16, &Graphics::onTimer, 0);

	// From GLUtilities.h, to enable keyIsdown().
	initKeymapManager();

	printError("Graphics::initResources()");
	return 1;
}

void Graphics::onDisplay(void) {
	GLfloat time = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 500;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	transHand = T(0, handY / 10, 0);
	if (time > 1000)
		easter(time);
	if (recording)
		glBindTexture(GL_TEXTURE_2D, red);
	else if(playing)
		glBindTexture(GL_TEXTURE_2D, green);
	else if(menuOpen){
		glBindTexture(GL_TEXTURE_2D, darkBlue);
		transHand = T(handX / 10, handY / 10, 0);
	}
	else
		glBindTexture(GL_TEXTURE_2D, white);

	drawObject(transHand, note, program);

	glBindTexture(GL_TEXTURE_2D, clef);
	drawObject(transPlane, plane, program);

	if (loadNewSign){
		LoadTGATextureSimple((char*)signTexturePath.c_str(), &signTexture);
		loadNewSign = false;
	}

	if (signShowing) {
		glBindTexture(GL_TEXTURE_2D, signTexture);
		drawObject(transSign, plane, program);
	}

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

void Graphics::easter(GLfloat time){
		transHand = Mult(T(handX / 10, handY / 10, 0), Ry(M_PI_2 * time));
		note = bunny;
}

void Graphics::showSign(std::string imagePath){
	signTexturePath = imagePath;
	signShowing = true;
	loadNewSign = true;
}

void Graphics::hideSign(void){
	signShowing = false;
}

void Graphics::openMenu(void){

	menuOpen = true;
}

void Graphics::closeMenu(void){
	menuOpen = false;
}
