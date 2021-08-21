#pragma once
/* Cylinder.h : This file contains the code necessary to represent
 *      a 3D Cylinder object in OpenGL. The overloaded constructor 
 *		requires:
 *				height,
 *				radius,
 *				x, y, and z for the center of the top,
 *				type of cylinder
 * 
 *				Modified to generate normals and not require calls
 *				GenCylinder and GenIndices
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
#include <gl/glew.h>

#include <vector>

namespace {
	/* Switchable determinator for types of cylinders to create.
	* NONE is a cylinder without top or bottom.
	* BOTH is a cylinder with both top and bottom.
	* BOTTOM is a cylinder with a bottom but no top.
	* TOP is a cylinder with a top but no bottom.
	*/
	enum CylinderType { BOTH, BOTTOM, TOP, NONE };
	using std::vector;
	using glm::vec3;
}

/* This class holds the data necessary to draw a 3D cylinder in OpenGL
*/
class Cylinder {
private:
	const float PI = 3.14159265359f;		// PI rounded
	const float PI2 = PI * 2;				// Twice PI rounded
	int m_numSlices = 16;					// Number of sections of the cylinder
	float m_radius;							// Radius of the cylinder
	float m_height;							// Height of the cylinder
	float originX;							// X coordinate for the center of the top
	float originY;							// Y coordinate for the center of the top
	float originZ;							// Z coordinate for the center of the top
	CylinderType m_type;					// Type of cylinder
	vector<GLfloat> vertices;				// Storage for the vertices
	vector<GLushort> indices;				// Storage for the indices
	vector<glm::vec2> circleXZ;				// Storage for x and z coordinate for slices around the cylinder
	vector<GLfloat> topCircle;				// Circle on top of cylinder
	vector<GLfloat> bottomCircle;			// Circle on bottom of cylinder

public:
	// Parameterized constructor for a cylidner
	Cylinder(float height, float radius, float x, float y, float z, CylinderType type);
	// Generate the vertices
	void GenCylinder();
	// Generate the indices
	void GenIndices(int curVertices);

	vector<GLfloat> GetVertices();			// Storage for the vertices
	vector<GLushort> GetIndices();			// Storage for the indices
};

// Parameterized constructor for a cylidner
Cylinder::Cylinder(float height, float radius, float x, float y, float z, CylinderType type) {
	// Store input values in fields
	m_height = height;
	m_radius = radius;
	originX = x;
	originY = y;
	originZ = z;
	m_type = type;
	GenCylinder();
}

// Generate the vertices for the cylinder
void Cylinder::GenCylinder() {
	GLfloat tempX;										// Temporary X value
	GLfloat tempZ;										// Temporary Z value
	GLfloat tempCos;									// Temporary Cos value
	GLfloat tempSin;									// Temporary Sin value
	vector<GLfloat> sines;
	vector<GLfloat> cosines;
	glm::vec3 tempNormal;
	glm::vec2 tempCircleXZ;
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 down(0.0f, -1.0f, 0.0f);
	// Loop for the number of slices requested
	for (int i = 0; i < m_numSlices; i++) {
		// Slice x = originX + radius * cosine (2*PI*Angle)
		GLfloat u = i / (GLfloat)m_numSlices;
		tempCos = cos(2 * PI * u);
		cosines.push_back(tempCos);
		tempX = originX + m_radius * tempCos;
		vertices.push_back(tempX);
		// Slice y value doesn't change
		vertices.push_back(originY);
		// Slice z = originZ + radius * sine (2*PI*Angle)
		tempSin = sin(2 * PI * u);
		sines.push_back(tempSin);
		tempZ = originZ + m_radius * tempSin;
		vertices.push_back(tempZ);
		tempNormal = glm::normalize(vec3(tempX, originY, tempZ) - vec3(originX, originY, originZ));
		vertices.insert(vertices.end(), { tempNormal.x, tempNormal.y, tempNormal.z });
		vertices.push_back(i / (float) m_numSlices);
		vertices.push_back(1.0f);
		vertices.push_back(tempX);
		// Slice bottom vertex is the same as top except subtract height
		vertices.push_back(originY - m_height);
		vertices.push_back(tempZ);
		vertices.insert(vertices.end(), { tempNormal.x, tempNormal.y, tempNormal.z });
		vertices.push_back(i / (float) m_numSlices);
		vertices.push_back(0.0f);

		// Store x and z for circle
		tempCircleXZ.x = tempX;
		tempCircleXZ.y = tempZ;
		circleXZ.push_back(tempCircleXZ);
	}
	// Add the original slice's vertices to close the cylinder walls

	GLfloat u = 0 / (GLfloat)m_numSlices;
	tempCos = cos(2 * PI * u);
	cosines.push_back(tempCos);
	tempX = originX + m_radius * tempCos;
	vertices.push_back(tempX);
	// Slice y value doesn't change
	vertices.push_back(originY);
	tempSin = sin(2 * PI * u);
	sines.push_back(tempSin);
	tempZ = originZ + m_radius * tempSin;
	vertices.push_back(tempZ);
	tempNormal = glm::normalize(vec3(tempX, originY, tempZ) - vec3(originX, originY, originZ));
	tempNormal = -tempNormal;
	vertices.insert(vertices.end(), { tempNormal.x, tempNormal.y, tempNormal.z });
	vertices.push_back(1.0f);
	vertices.push_back(1.0f);
	vertices.push_back(tempX);
	// Slice bottom vertex is the same as top except subtract height
	vertices.push_back(originY - m_height);
	vertices.push_back(tempZ);
	tempNormal = glm::normalize(vec3(tempX, originY - m_height, tempZ) - vec3(originX, originY, originZ));
	tempNormal = -tempNormal;
	vertices.insert(vertices.end(), { tempNormal.x, tempNormal.y, tempNormal.z });
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);
	tempCircleXZ.x = tempX;
	tempCircleXZ.y = tempZ;
	circleXZ.push_back(tempCircleXZ);


	float angleIncrement = PI2 / m_numSlices;
	// Add vertices for bottom
	if ((m_type == BOTTOM) || (m_type == BOTH)) {
		bottomCircle.insert(bottomCircle.end(), {originX, originY - m_height, originZ, down.x, down.y, down.z, 0.5f, 0.5f });
		for (unsigned int i = 0; i < circleXZ.size(); i++) {
			bottomCircle.insert(bottomCircle.end(), { circleXZ.at(i).x, originY - m_height, circleXZ.at(i).y,down.x, down.y, down.z, (cosines.at(i) * 0.5f) + 0.5f, (sines.at(i) * 0.5f) + 0.5f });
		}
	}
	// Add vertices for top
	if ((m_type == TOP) || (m_type == BOTH)) {
		topCircle.insert(topCircle.end(), { originX, originY, originZ, up.x, up.y, up.z, 0.5f, 0.5f });
		for (unsigned int i = 0; i < circleXZ.size(); i++) {
			topCircle.insert(topCircle.end(), { circleXZ.at(i).x, originY, circleXZ.at(i).y, up.x, up.y, up.z, (cosines.at(i)* 0.5f) +0.5f, (sines.at(i) * 0.5f) + 0.5f });
		}
	}
	GenIndices(0);
}

// Generate the indices to add to end of already completed shapes
void Cylinder::GenIndices(int curIndices) {
	for (int i = 0; i < m_numSlices; i++) {
		// Loops through the slices adding the indices
		indices.push_back(2 * i + curIndices);
		indices.push_back(2 * i + curIndices + 1);
		indices.push_back(2 * i + curIndices + 2);
		indices.push_back(2 * i + curIndices + 1);
		indices.push_back(2 * i + curIndices + 2);
		indices.push_back(2 * i + curIndices + 3);
	}

	// Determine if bottom needs to be drawn
	if ((m_type == BOTTOM) || (m_type == BOTH)) {
		/* Add indices to draw bottom. Bottom is drawn using veretices from
		* tops of slices and the origin to make triangles.
		*/
		int startIndex = vertices.size()/8;
		vertices.insert(vertices.end(), bottomCircle.begin(), bottomCircle.end());

		for (int i = 1; i <= m_numSlices; i++) {
			indices.insert(indices.end(), { (GLushort)startIndex, (GLushort)(startIndex + i), (GLushort)(startIndex + i + 1) });
		}

	}
	// Determine if top needs to be drawn
	if ((m_type == TOP) || (m_type == BOTH)) {
		/* Add indices to draw top. Top is drawn using veretices from
		* bottoms of slices and the origin translated down by height
		* to make triangles.
		*/
		int startIndex = vertices.size() / 8;
		vertices.insert(vertices.end(), topCircle.begin(), topCircle.end());

		for (int i = 1; i <= m_numSlices; i++) {
			indices.insert(indices.end(), { (GLushort)startIndex, (GLushort)(startIndex + i), (GLushort)(startIndex + i + 1) });
		}		
	}
}
// Return the vertices
vector<GLfloat> Cylinder::GetVertices() {
	return vertices;
}
// Return the indices
vector<GLushort> Cylinder::GetIndices() {
	return indices;
}
