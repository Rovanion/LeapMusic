#include <math.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "GLUtilities.h"
#include "LoadObject.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "Graphics.h"

#define near 1.0
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// vertex array object
unsigned int vertexArrayObjID;
GLuint program;

Model *bunny;
mat4 transBunny;

GLuint concrete;
GLuint grass;

mat4 lookMatrix;
vec3 cameraPos;
vec3 cameraTarget;
vec3 cameraNormal;
vec3 cameraDirection;


void initGraphics(void)
{
  cameraPos = (vec3){0.0f, 0.0f, 10.0f};
  cameraTarget = (vec3){0.0f, 0.0f, 0.0f};
  cameraNormal = (vec3){0.0f, 1.0f, 0.0f};
  lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);

	dumpInfo();

	bunny = LoadModelPlus("src/graphics/models/bunnyplus.obj");
	transBunny = Mult(T(0, 2, 0), Ry(M_PI/2));

	// Load textures
	LoadTGATextureSimple("src/graphics/textures/conc.tga", &concrete);
	LoadTGATextureSimple("src/graphics/textures/grass.tga", &grass);

	// GL inits
	glClearColor(0.0,0.0,0.0,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);

	// Load and compile shader
	program = loadShaders("src/graphics/shaders/main.vert",
												"src/graphics/shaders/main.frag");

	GLfloat projectionMatrix[] =
		{ 2.0f*near/(right-left), 0.0f,
		 (right+left)/(right-left), 0.0f,
		 0.0f, 2.0f*near/(top-bottom),
		 (top+bottom)/(top-bottom), 0.0f,
		 0.0f, 0.0f, -(far + near)/(far - near),
		 -2*far*near/(far - near),
		 0.0f, 0.0f, -1.0f, 0.0f };

	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"),
										 1, GL_TRUE, projectionMatrix);

	printError("Light and shit");
  Point3D lightSourcesColorsArr[] =
		{ {1.0f, 0.0f, 0.0f},   // Red light
			{0.0f, 1.0f, 0.0f},   // Green light
			{0.0f, 0.0f, 1.0f},   // Blue light
			{1.0f, 1.0f, 1.0f} }; // White light

	GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
	GLint isDirectional[] = {0,0,1,1};
	Point3D lightSourcesDirectionsPositions[] =
		{ {10.0f, 5.0f, 0.0f}, // Red light, positional
			{0.0f, 5.0f, 10.0f}, // Green light, positional
			{-1.0f, 0.0f, 0.0f}, // Blue light along X
			{0.0f, 0.0f, -1.0f} }; // White light along Z

	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
							 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"),
							 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"),
							 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"),
							 4, isDirectional);

}

void onTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, &onTimer, value);
}

void display(void)
{
	printError("pre display");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraPos = moveOnKeyInputRelativeCamera(cameraPos);
	lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookMatrix"),
										 1, GL_TRUE, lookMatrix.m);

	glBindTexture(GL_TEXTURE_2D, concrete);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	drawObject(transBunny,bunny);

	glBindTexture(GL_TEXTURE_2D, grass);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);

	printError("display");
	glutSwapBuffers();
}

void drawObject(mat4 transform, Model* model)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "transform"),
										 1, GL_TRUE, transform.m);
	DrawModel(model, program, "in_Position",
						"in_Normal", "in_TexCoord");
}

int graphicsMain(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Lab 3");
	glutDisplayFunc(display);
	initKeymapManager();
	glutPassiveMotionFunc(handleMouse);
	initGraphics();
	glutTimerFunc(16, &onTimer, 0);
	glutMainLoop();
}

void handleMouse(int x, int y)
{
  cameraTarget = (vec3) {cos((float)x / 400 * M_PI * 2) * 20,
												 cos((float)y / 400 * M_PI * 2) * 20,
												 sin((float)x / 400 * M_PI * 2) * 20};
	cameraTarget = VectorAdd(cameraTarget, cameraPos);

  lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);
	cameraDirection = Normalize(VectorSub(cameraTarget, cameraPos));
}



vec3 moveOnKeyInputRelativeCamera(vec3 in)
{
	vec3 forward;
	vec3 leftV;
	forward = ScalarMult(cameraDirection, 0.5f);
	leftV = ScalarMult(CrossProduct(cameraDirection, cameraNormal), 0.5f);

  if(keyIsDown('w')) {
    in.x += forward.x;
    in.y += forward.y;
    in.z += forward.z;
	}
  else if (keyIsDown('s')) {
    in.x -= forward.x;
    in.y -= forward.y;
    in.z -= forward.z;
	}

  if(keyIsDown('a')){
		in.x -= leftV.x;
    in.y -= leftV.y;
    in.z -= leftV.z;
	}
  else if(keyIsDown('d')){
		in.x += leftV.x;
    in.y += leftV.y;
    in.z += leftV.z;
	}

  if(keyIsDown('q'))
    in.z += 0.1;
  else if(keyIsDown('e'))
    in.z -= 0.1;

  return in;
}
