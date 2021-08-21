#pragma once
/* Cuboid.h : This file contains the code necessary to represent
 *      a 3D Cuboid object in OpenGL. The overloaded constructor
 *		requires:
 *				length,
 *				width,
 *				height,
 *				radius,
 *				x, y, and z for the back left top vertex
 * 
 *				Modified to create normals and not require calls to GenCuboid or GenIndices
 *
 *Author:      David Smith
 *Course:      CS-320
 *Instructor:  E. Rodriguez
 *Date:        August 8, 2021
 *Version:     3.0
 */

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

namespace {
	using std::vector;
}

// This class contains the data necessary to draw a cuboid in OpenGL
class Cuboid {
private:
	 float m_length;			// Length of cube (z axis)
	 float m_width;				// Width of cube (x axis)
	 float m_height;			// Height of cube (y axis)
	 float originX;				// Back left top vertex x
	 float originY;				// Back left top vertex y
	 float originZ;				// Back left top vertex z
	 GLfloat tiles_side;		// Number of times to tile texture horizontally on side surfaces
	 GLfloat tiles_frontBack;	// Number of times to tile texture horizontally on front and back surfaces
	 GLfloat tiles_topBottom;	// Number of times to tile texture horizontally on top and bottom
	 vector<GLfloat> vertices;	// Storage for vertices
	 vector<GLushort> indices;	// Storage for indices

public:
	// Parameterized constructor
	Cuboid(float length, float width, float height, float x, float y, float z, int m_tiles);
	// Generate the vertices
	void GenCuboid();
	// Generate the indices;
	void GenIndices(GLushort curIndices);

	vector<GLfloat> GetVertices();
	vector<GLushort> GetIndices();
	void SetTiles(GLfloat sides, GLfloat frontback, GLfloat topbottom);
};

// Parameterized constructor for a cuboid
Cuboid::Cuboid(float length, float width, float height, float x, float y, float z, int m_tiles) {
	// Store input values in fields
	m_length = length;
	m_width = width;
	m_height = height;
	originX = x;
	originY = y;
	originZ = z;
	tiles_topBottom = 1;
	tiles_frontBack = 1;
	tiles_side = 1;
	GenCuboid();
}

// Add vertices to vertex
// Multiplying x value of texture coordinate ((float)i * tiles) to create a tiling effect on the surfaces of the end table
void Cuboid::GenCuboid() {
	vec3 up(0.0f, 1.0f, 0.0f);
	vec3 down(0.0f, -1.0f, 0.0f);
	vec3 left(1.0f, 0.0f, 0.0f);
	vec3 right(-1.0f, 0.0f, 0.0f);
	vec3 front(0.0f, 0.0f, 1.0f);
	vec3 back(0.0f, 0.0f, -1.0f);

	vertices.insert(vertices.end(), { originX, originY, originZ, up.x, up.y, up.z, 0.0f, 1.0f * tiles_frontBack});						// Back left top
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ, up.x, up.y, up.z, 1.0f, 1.0f * tiles_frontBack });			// Back right top
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ + m_length, up.x, up.y, up.z, 1.0f, 0.0f });					// Front right top
	vertices.insert(vertices.end(), { originX, originY, originZ + m_length, up.x, up.y, up.z, 0.0f, 0.0f });								// Front left top
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ, down.x, down.y, down.z, 0.0f, 1.0f * tiles_frontBack });			// Back left bottom
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ, down.x, down.y, down.z, 1.0f, 1.0f * tiles_frontBack });	// Back right bottom
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ + m_length, down.x, down.y, down.z, 1.0f, 0.0f });		// Back left bottom
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ + m_length, down.x, down.y, down.z, 0.0f, 0.0f });					// Back left bottom
	vertices.insert(vertices.end(), { originX, originY, originZ, back.x, back.y, back.z, 0.0f, 1.0f * tiles_frontBack});						// Back left top
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ, back.x, back.y, back.z, 1.0f, 1.0f * tiles_frontBack });			// Back right top
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ, back.x, back.y, back.z, 1.0f, 0.0f });					// Back right bottom
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ, back.x, back.y, back.z, 0.0f, 0.0f });								// Back left bottom
	vertices.insert(vertices.end(), { originX, originY, originZ + m_length, front.x, front.y, front.z, 0.0f, 1.0f * tiles_frontBack });			// Front left top
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ + m_length, front.x, front.y, front.z, 1.0f, 1.0f * tiles_frontBack });	// Front right top
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ + m_length, front.x, front.y, front.z, 1.0f, 0.0f });		// Front right bottom
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ + m_length, front.x, front.y, front.z, 0.0f, 0.0f });					// Front left bottom
	vertices.insert(vertices.end(), { originX, originY, originZ, right.x, right.y, right.z, 0.0f, 1.0f * tiles_side});							// Back left top
	vertices.insert(vertices.end(), { originX, originY, originZ + m_length, right.x, right.y, right.z, 1.0f, 1.0f * tiles_side });				// Front left top
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ + m_length, right.x, right.y, right.z, 1.0f, 0.0f });					// Front left bottom
	vertices.insert(vertices.end(), { originX, originY - m_height, originZ, right.x, right.y, right.z, 0.0f, 0.0f });								// Back left bottom
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ, left.x, left.y, left.z, 0.0f, 1.0f * tiles_side });					// Back right top
	vertices.insert(vertices.end(), { originX + m_width, originY, originZ + m_length, left.x, left.y, left.z, 1.0f, 1.0f * tiles_side });		// Front right top
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ + m_length, left.x, left.y, left.z, 1.0f, 0.0f });		// Front right bottom
	vertices.insert(vertices.end(), { originX + m_width, originY - m_height, originZ, left.x, left.y, left.z, 0.0f, 0.0f });					// Back right bottom

	GenIndices(0);

}

// Create indices based on curIndices already existing
void Cuboid::GenIndices(GLushort curIndices) {
	indices.insert(indices.end(), {	curIndices, (GLuint)(curIndices + 1), (GLuint)(curIndices + 2),						// Half of top, back right
									(GLuint)(curIndices + 3), (GLuint)(curIndices + 2), curIndices,						// Half of top, front left
									(GLuint)(curIndices + 4), (GLuint)(curIndices + 5), (GLuint)(curIndices + 6),		// Half of bottom, back right
									(GLuint)(curIndices + 4), (GLuint)(curIndices + 6), (GLuint)(curIndices + 7),		// Half of bottom, front left
									(GLuint)(curIndices + 8), (GLuint)(curIndices + 9), (GLuint)(curIndices + 10),		// Half of back, upper left
									(GLuint)(curIndices + 8), (GLuint)(curIndices + 10), (GLuint)(curIndices + 11),		// Half of back, lower right
									(GLuint)(curIndices + 12), (GLuint)(curIndices + 13), (GLuint)(curIndices + 14),	// Half of front, upper right
									(GLuint)(curIndices + 12), (GLuint)(curIndices + 14), (GLuint)(curIndices + 15),	// Half of front, lower left
									(GLuint)(curIndices + 16), (GLuint)(curIndices + 17), (GLuint)(curIndices + 18),	// Half of right, upper back
									(GLuint)(curIndices + 16), (GLuint)(curIndices + 18), (GLuint)(curIndices + 19),	// Half of right, lower front
									(GLuint)(curIndices + 20), (GLuint)(curIndices + 21), (GLuint)(curIndices + 22),	// Half of left, upper back
									(GLuint)(curIndices + 20), (GLuint)(curIndices + 22), (GLuint)(curIndices + 23),	// Half of left, lower front
									});
}
// Return vertices
vector<GLfloat> Cuboid::GetVertices() {
	return vertices;
}
// Return indices
vector<GLushort> Cuboid::GetIndices() {
	return indices;
}
void Cuboid::SetTiles(GLfloat sides, GLfloat frontback, GLfloat topbottom) {
	if (sides <= 0) {
		tiles_side = 1;
	}
	else {
		tiles_side = sides;
	}
	if (frontback <= 0) {
		tiles_frontBack = 1;
	}
	else {
		tiles_frontBack = frontback;
	}
	if (topbottom <= 0) {
		tiles_topBottom = 1;
	}
	else {
		tiles_topBottom = topbottom;
	}
}
