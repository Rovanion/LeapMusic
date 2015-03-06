// VectorUtils3 header
// See source for more information

#ifndef VECTORUTILS3
#define VECTORUTILS3

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define Vector3f Point3D
#define Matrix3f Matrix3D
#define Matrix4f Matrix4D

// GLSL-style
#define Point3D vec3
#define Matrix3D mat3
#define Matrix4D mat4

	// vec3 is very useful
	typedef struct vec3
	{
		GLfloat x, y, z;
	}
	vec3, *vec3Ptr;

	// vec4 is not as useful. Can be a color with alpha, or a quaternion, but IMHO you
	// rarely need homogenous coordinate vectors on the CPU.
	typedef struct vec4
	{
		GLfloat x, y, z, w; // w or h
	}
	vec4, *vec4Ptr;
	typedef struct mat4
	{
		GLfloat m[16];
	} mat4;
	typedef struct mat3
	{
		GLfloat m[9];
	} mat3;

#ifdef __cplusplus
extern "C" {
#endif

//	void CopyVector(vec3 *v, vec3 *dest); // Will probably be removed
	vec3 SetVector(GLfloat x, GLfloat y, GLfloat z);
// Basic vector operations on vec3's. (vec4 not included since I never need them.)
	vec3 VectorSub(vec3 a, vec3 b);
	vec3 VectorAdd(vec3 a, vec3 b);
	vec3 CrossProduct(vec3 a, vec3 b);
	GLfloat DotProduct(vec3 a, vec3 b);
	vec3 ScalarMult(vec3 a, GLfloat s);
	GLfloat Norm(vec3 a);
	vec3 Normalize(vec3 a);
	vec3 CalcNormalVector(vec3 a, vec3 b, vec3 c);
	void SplitVector(vec3 v, vec3 n, vec3 *vn, vec3 *vp);

// Matrix operations primarily on 4x4 matrixes!
// Row-wise by default but can be configured to column-wise (see SetTransposed)

	mat4 IdentityMatrix();
	mat4 Rx(GLfloat a);
	mat4 Ry(GLfloat a);
	mat4 Rz(GLfloat a);
	mat4 T(GLfloat tx, GLfloat ty, GLfloat tz);
	mat4 S(GLfloat sx, GLfloat sy, GLfloat sz);
	mat4 Mult(mat4 a, mat4 b); // dest = a * b - rename to MultMat4 considered but I don't like to make the name of the most common operation longer
	// but for symmetry, MultMat4 is made a synonym:
	#define MultMat4 Mult

	// Was MatrixMultPoint3D
	vec3 MultVec3(mat4 a, vec3 b); // result = a * b
	vec4 MultVec4(mat4 a, vec4 b);
//	void CopyMatrix(GLfloat *src, GLfloat *dest); // Will probably be removed

// Mat3 operations (new)
	mat3 MultMat3(mat3 a, mat3 b); // m = a * b
	vec3 MultMat3Vec3(mat3 a, vec3 b); // result = a * b

	void OrthoNormalizeMatrix(mat4 *R);
	mat4 Transpose(mat4 m);
//	mat4 TransposeRotation(mat4 m); // Will probably be removed
	mat3 TransposeMat3(mat3 m);
	mat4 ArbRotate(vec3 axis, GLfloat fi);
	mat4 CrossMatrix(vec3 a);
	mat4 MatrixAdd(mat4 a, mat4 b);

// Configure, i.e. if you want matrices to be column-wise
	void SetTransposed(char t);

// GLU replacement functions
	mat4 lookAtv(vec3 p, vec3 l, vec3 v);
	mat4 lookAt(GLfloat px, GLfloat py, GLfloat pz,
			GLfloat lx, GLfloat ly, GLfloat lz,
			GLfloat vx, GLfloat vy, GLfloat vz);
	mat4 perspective(float fovyInDegrees, float aspectRatio,
                      float znear, float zfar);
	mat4 frustum(float left, float right, float bottom, float top,
                  float znear, float zfar);

// For creating a normal matrix
	mat3 InvertMat3(mat3 in);
	mat3 InverseTranspose(mat4 in);
	mat4 InvertMat4(mat4 a);

// Simple conversions
	mat3 mat4tomat3(mat4 m);
	mat4 mat3tomat4(mat3 m);
	vec3 vec4tovec3(vec4 v);
	vec4 vec3tovec4(vec3 v);
#ifdef __cplusplus
}
#endif

#endif
