#include "../include/Leap.h"
#include "Matte.h"

using namespace Leap;

/**
 * Returns true if a and b are within epsilon of each other.
 */
bool Matte::fuzzyEquals(double a, double b, double epsilon){
	return fabs(a - b) < epsilon;
}

/**
 * Returns true if every element of the vectors a and b are within
 * epsilon of each other.
 */
bool Matte::fuzzyEquals(Vector a, Vector b, double epsilon){
	Vector diff = a - b;
	return  diff.x < epsilon && diff.y < epsilon && diff.z < epsilon;
}

/**
 * Returns true if the smallest angle betwene the two vectors are
 * within epsilon of each other.
 */
bool Matte::fuzzyAngleEquals(Vector a, Vector b, double epsilon){
	return a.angleTo(b) < epsilon;
}