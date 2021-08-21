/* Cylinder.h : This file contains the code necessary to represent
 *      a 3D Sphere object in OpenGL. The overloaded constructor
 *		requires:
 *				height,
 *				radius,
 *				x, y, and z for the center
 * 
 *				Modified to generate normals and not require calls
 *				to GenSphere and GenIndices
 *
 *Author:      David Smith
 *Course:      CS-320
 *Instructor:  E. Rodriguez
 *Date:        August 8, 2021
 *Version:     3.0
 */

#pragma once
// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gl/glew.h>

#include <vector>

namespace {
	using std::vector;
	using glm::vec2;
	using glm::vec3;
	using glm::normalize;
}

/* This class holds the data necessary to draw a 3D cylinder in OpenGL
*/
class Sphere {
private:
	const float PI = 3.14159265359f;		// PI rounded
	float PI_2 = PI / 2.0f;					// Half of PI rounded
	int numRings;							// Number of rings in sphere
	int numSectors;							// Number of sectors in sphere
	float m_radius;							// Radius of the cylinder
	float originX;							// X coordinate for the center of the top
	float originY;							// Y coordinate for the center of the top
	float originZ;							// Z coordinate for the center of the top
	vector<GLfloat> vertices;				// Storage for the vertices
	vector<GLushort> indices;				// Storage for the indices
	vector<vec2> UVs;				// Temporary storage for UVs
	vector<vec3> normals;					// Temporary storage for normals
	vector<vec3> vertexData;				// Temporary storage for vertices

public:
	// Parameterized constructor for a sphere
	Sphere(float radius, int rings, int sectors, float x, float y, float z);
	// Generate the vertices
	void GenSphere();
	// Generate the indices
	void GenIndices(int r, int s);
	// Retrieve the vertices
	vector<GLfloat> GetVertices();
	// Retrieve the indices
	vector<GLushort> GetIndices();
};

// Parameterized constructor
Sphere::Sphere(float radius, int rings, int sectors, float x, float y, float z) {
	numRings = rings;
	numSectors = sectors;
	m_radius = radius;
	originX = x;
	originY = y;
	originZ = z;
	GenSphere();
}

// Generate the sphere
void Sphere::GenSphere() {
	const float R = 1.0f / (float)(numRings - 1);
	const float S = 1.0f / (float)(numSectors - 1);

	for (int ring = 0; ring < numRings; ring++) {
		for (int sector = 0; sector < numSectors; sector++) {
			const float x = cos(2 * PI * sector * S) * sin(PI * ring * R);
			const float y = sin(-PI_2 + PI * ring * R);
			const float z = sin(2 * PI * sector * S) * sin(PI * ring * R);

			UVs.push_back(vec2(sector * S, ring * R));
			vertexData.push_back(vec3(x, y, z) * m_radius);
			normals.push_back(normalize(vec3(x, y, z) - vec3(originX, originY, originZ)));
			GenIndices(ring, sector);
		}
	}
	// Add data to vertices vector
	for (unsigned int i = 0; i < vertexData.size(); i++) {
		vertices.insert(vertices.end(), {	vertexData.at(i).x, vertexData.at(i).y, vertexData.at(i).z,
											normals.at(i).x, normals.at(i).y, normals.at(i).z,
											UVs.at(i).x, UVs.at(i).y });
	}

}

// Generate the indices
void Sphere::GenIndices(int ring, int sector) {
	int curRow = ring * numSectors;
	int nextRow = (ring + 1) * numSectors;
	int nextS = (sector + 1) % numSectors;

	indices.push_back(curRow + sector);
	indices.push_back(nextRow + sector);
	indices.push_back(nextRow + nextS);

	indices.push_back(curRow + sector);
	indices.push_back(nextRow + nextS);
	indices.push_back(curRow + nextS);
}

// Retrieve the vertices
vector<GLfloat> Sphere::GetVertices() {
	return vertices;
}

// Retrieve the indices
vector<GLushort> Sphere::GetIndices() {
	return indices;
}