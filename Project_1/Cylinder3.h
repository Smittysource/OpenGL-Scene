#pragma once
#include <iostream>
#include <vector>
#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace {
    using std::vector;
}

class Cylinder
{
public:
    Cylinder(float radius, int numSlices, float height);

    void render();
    void renderPoints();

    /**
     * Gets cylinder radius.
     */
    float getRadius();

    /**
     * Gets number of cylinder slices.
     */
    int getSlices();

    /**
     * Gets cylinder height.
     */
    float getHeight();

    /*
    * Get vertex data
    */
    void genVertexData();

    /*
    * Get VAO for rendering
    */
    GLuint getVAO();

    /*
    * Get number of vertices
    */
    GLuint getSidesVertices();
    GLuint getTopBottomVertices();
    

private:
    float radius; // Cylinder radius (distance from the center of cylinder to surface)
    int numSlices; // Number of cylinder slices
    float height; // Height of the cylinder

    int numVerticesSide; // How many vertices to render side of the cylinder
    int numVerticesTopBottom; // How many vertices to render top / bottom of the cylinder
    int numVerticesTotal; // Just a sum of both numbers above
    GLuint vao; // VAO ID from OpenGL
    GLuint vbo; // Our VBO wrapper class holding static mesh data
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;

    vector<GLfloat> vertexData;

    void initializeData();
};

Cylinder::Cylinder(float radius, int numSlices, float height)
{
    this->numSlices = numSlices;
    this->radius = radius;
    this->height = height;
    initializeData();
}

float Cylinder::getRadius()
{
    return radius;
}

int Cylinder::getSlices()
{
    return numSlices;
}

float Cylinder::getHeight()
{
    return height;
}

void Cylinder::initializeData()
{
    // Calculate and cache numbers of vertices
    numVerticesSide = (numSlices + 1) * 2;
    numVerticesTopBottom = numSlices + 2;
    numVerticesTotal = numVerticesSide + numVerticesTopBottom * 2;
    // Generate VAO and VBO for vertex attributes
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Pre-calculate sines / cosines for given number of slices
    const auto sliceAngleStep = 2.0f * glm::pi<float>() / static_cast<float>(numSlices);
    auto currentSliceAngle = 0.0f;
    std::vector<float> sines, cosines;
    for (auto i = 0; i <= numSlices; i++)
    {
        sines.push_back(sin(currentSliceAngle));
        cosines.push_back(cos(currentSliceAngle));

        // Update slice angle
        currentSliceAngle += sliceAngleStep;
    }

    // Pre-calculate X and Z coordinates
    std::vector<float> x;
    std::vector<float> z;
    for (auto i = 0; i <= numSlices; i++)
    {
        x.push_back(cosines[i] * radius);
        z.push_back(sines[i] * radius);
    }

    // Add cylinder side vertices
    for (auto i = 0; i <= numSlices; i++)
    {
        glm::vec3 topPosition = glm::vec3(x[i], height / 2.0f, z[i]);
        glm::vec3 bottomPosition = glm::vec3(x[i], -height / 2.0f, z[i]);
        vertices.push_back(topPosition);
        vertices.push_back(bottomPosition);
    }
    // Add top cylinder cover
    glm::vec3 topCenterPosition(0.0f, height / 2.0f, 0.0f);
    vertices.push_back(topCenterPosition);
    for (int i = 0; i <= numSlices; i++)
    {
        glm::vec3 topPosition = glm::vec3(x[i], height / 2.0f, z[i]);
        vertices.push_back(topPosition);
    }

    // Add bottom cylinder cover
    glm::vec3 bottomCenterPosition(0.0f, -height / 2.0f, 0.0f);
    vertices.push_back(bottomCenterPosition);
    for (int i = 0; i <= numSlices; i++)
    {
        const auto bottomPosition = glm::vec3(x[i], -height / 2.0f, -z[i]);
        vertices.push_back(bottomPosition);
    }
    
    // Pre-calculate step size in texture coordinate U
    // I have decided to map the texture twice around cylinder, looks fine
    const auto sliceTextureStep = 1.0f / static_cast<float>(numSlices);

    auto currentSliceTexCoord = 0.0f;
    for (auto i = 0; i <= numSlices; i++)
    {
        UVs.push_back(glm::vec2(currentSliceTexCoord, 1.0f));
        UVs.push_back(glm::vec2(currentSliceTexCoord, 0.0f));
        
        // Update texture coordinate of current slice 
        currentSliceTexCoord += sliceTextureStep;
    }

    // Generate circle texture coordinates for cylinder top cover
    glm::vec2 topBottomCenterTexCoord(0.5f, 0.5f);
    UVs.push_back(topBottomCenterTexCoord);
    for (auto i = 0; i <= numSlices; i++) {
        UVs.push_back(glm::vec2(topBottomCenterTexCoord.x + sines[i] * 0.5f, topBottomCenterTexCoord.y + cosines[i] * 0.5f));
    }
    // Generate circle texture coordinates for cylinder bottom cover
    UVs.push_back(topBottomCenterTexCoord);
    for (auto i = 0; i <= numSlices; i++) {
        UVs.push_back(glm::vec2(topBottomCenterTexCoord.x + sines[i] * 0.5f, topBottomCenterTexCoord.y + cosines[i] * 0.5f));
    }
    
    // Add normals for outside of cylinder
    for (auto i = 0; i <= numSlices; i++) {
        normals.push_back(glm::vec3(cosines[i], 0.0f, sines[i]));
        normals.push_back(glm::vec3(cosines[i], 0.0f, sines[i]));
    }
    // Add normal for every vertex of cylinder top cover
    for(int i = 0; i < numVerticesTopBottom; i++)
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    // Add normal for every vertex of cylinder bottom cover
    for (int i = 0; i < numVerticesTopBottom; i++)
        normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    genVertexData();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each
    glBufferData(GL_ARRAY_BUFFER, numVerticesSide, vertexData.data(), GL_STATIC_DRAW);
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    

/*    // Finally upload data to the GPU
    vbo.bindVBO();
    vbo.uploadDataToGPU(GL_STATIC_DRAW);
    setVertexAttributesPointers(numVerticesTotal);
    */
}
/*
void Cylinder::render() const
{
    glBindVertexArray(_vao);

    // Render cylinder side first
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerticesSide);

    // Render top cover
    glDrawArrays(GL_TRIANGLE_FAN, numVerticesSide, numVerticesTopBottom);

    // Render bottom cover
    glDrawArrays(GL_TRIANGLE_FAN, numVerticesSide + numVerticesTopBottom, numVerticesTopBottom);
}

void Cylinder::renderPoints() const
{
    // Just render all points as they are stored in the VBO
    glBindVertexArray(_vao);
    glDrawArrays(GL_POINTS, 0, numVerticesTotal);
}*/

void Cylinder::genVertexData() {
    std::cout << "Vertices: " << vertices.size() << "Normals: " << normals.size() << "UVs: " << UVs.size() << std::endl;
    for (unsigned int i = 0; i < vertices.size(); i++) {
        vertexData.insert(vertexData.end(), {vertices.at(i).x, vertices.at(i).y, vertices.at(i).z, normals.at(i).x, normals.at(i).y, normals.at(i).z, UVs.at(i).x, UVs.at(i).y});
    }
    std::cout << vertexData.size();
}

/*
* Get VAO for rendering
*/
GLuint Cylinder::getVAO() {
    return vao;
}

/*
* Get number of vertices
*/
GLuint Cylinder::getSidesVertices() {
    return numVerticesSide;
}
GLuint Cylinder::getTopBottomVertices() {
    return numVerticesTopBottom;
}
